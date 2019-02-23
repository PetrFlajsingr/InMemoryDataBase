//
// Created by Petr Flajsingr on 2019-02-08.
//

#include <MemoryViewDataSet.h>

DataSets::MemoryViewDataSet::MemoryViewDataSet(std::string_view dataSetName,
                                               const std::vector<std::string> &fieldNames,
                                               const std::vector<ValueType> &fieldTypes,
                                               const std::vector<std::pair<int,
                                                                           int>> &fieldIndices)
    : ViewDataSet(dataSetName) {
  createFields(fieldNames, fieldTypes, fieldIndices);
}

void DataSets::MemoryViewDataSet::createFields(const std::vector<std::string> &columns,
                                               const std::vector<ValueType> &types,
                                               const std::vector<std::pair<int,
                                                                           int>> &fieldIndices) {
  createNullRows(fieldIndices, types);
  for (gsl::index i = 0; i < columns.size(); ++i) {
    auto index =
        (fieldIndices[i].first << BaseField::maskTableShift)
            + fieldIndices[i].second;
    fields.emplace_back(FieldFactory::Get().CreateField(
        columns[i],
        index,
        types[i],
        this));
  }
  columnCount = fields.size();
}

void DataSets::MemoryViewDataSet::open(DataProviders::BaseDataProvider &dataProvider,
                                       const std::vector<ValueType> &fieldTypes) {
  throw NotImplementedException();
}

void DataSets::MemoryViewDataSet::openEmpty(const std::vector<std::string> &fieldNames,
                                            const std::vector<ValueType> &fieldTypes) {
  throw NotImplementedException();
}

void DataSets::MemoryViewDataSet::close() {}

void DataSets::MemoryViewDataSet::first() {
  currentRecord = 1;
  setFieldValues(currentRecord);
}

void DataSets::MemoryViewDataSet::last() {
  currentRecord = data.size() - 2;
  setFieldValues(currentRecord);
}

bool DataSets::MemoryViewDataSet::next() {
  currentRecord++;
  if (currentRecord >= data.size() - 1) {
    return false;
  }
  setFieldValues(currentRecord);
  return true;
}

bool DataSets::MemoryViewDataSet::previous() {
  currentRecord--;
  if (currentRecord <= 0) {
    return false;
  }
  setFieldValues(currentRecord);
  return true;
}

bool DataSets::MemoryViewDataSet::isFirst() const {
  return currentRecord == 1;
}

bool DataSets::MemoryViewDataSet::isBegin() const {
  return currentRecord == 0;
}

bool DataSets::MemoryViewDataSet::isLast() const {
  return currentRecord == data.size() - 2;
}

bool DataSets::MemoryViewDataSet::isEnd() const {
  return currentRecord == data.size() - 1;
}

DataSets::BaseField *DataSets::MemoryViewDataSet::fieldByName(std::string_view name) const {
  if (auto it = std::find_if(fields.begin(),
                             fields.end(),
                             [name](const std::shared_ptr<BaseField> field) {
                               return field->getName() == name;
                             }); it != fields.end()) {
    return (*it).get();
  }
  std::string errMsg = "Field named \"" + std::string(name)
      + "\" not found. DataSets::MemoryViewDataSet::fieldByName";
  throw InvalidArgumentException(errMsg.c_str());
}

DataSets::BaseField *DataSets::MemoryViewDataSet::fieldByIndex(gsl::index index) const {
  return fields.at(index).get();
}

std::vector<DataSets::BaseField *> DataSets::MemoryViewDataSet::getFields() const {
  std::vector<BaseField *> result;
  if (allowedFields.empty()) {
    std::transform(fields.begin(),
                   fields.end(),
                   std::back_inserter(result),
                   [](const std::shared_ptr<BaseField> field) {
                     return field.get();
                   });
  } else {
    return allowedFields;
  }
  return result;
}

std::vector<std::string> DataSets::MemoryViewDataSet::getFieldNames() const {
  std::vector<std::string> result;
  if (allowedFields.empty()) {
    std::transform(fields.begin(),
                   fields.end(),
                   std::back_inserter(result),
                   [](const std::shared_ptr<BaseField> field) {
                     return std::string(field->getName());
                   });
  } else {
    std::transform(allowedFields.begin(),
                   allowedFields.end(),
                   std::back_inserter(result),
                   [](const BaseField *field) {
                     return std::string(field->getName());
                   });
  }
  return result;
}

gsl::index DataSets::MemoryViewDataSet::getCurrentRecord() const {
  return currentRecord;
}

void DataSets::MemoryViewDataSet::append() {
  throw NotImplementedException();
}

void DataSets::MemoryViewDataSet::append(DataProviders::BaseDataProvider &dataProvider) {
  throw NotImplementedException();
}

void DataSets::MemoryViewDataSet::sort(DataSets::SortOptions &options) {
  std::vector<std::function<int8_t(const DataSetRow *, const DataSetRow *)>>
      compareFunctions;

  std::transform(options.options.begin(),
                 options.options.end(),
                 std::back_inserter(compareFunctions),
                 [](const SortItem &option) {
                   return option.field->getCompareFunction();
                 });

  auto optionArray = options.options;
  auto compareFunction =
      [this, &optionArray, &compareFunctions](const std::vector<DataSetRow *> &a,
                                              const std::vector<DataSetRow *> &b) {
        for (gsl::index i = 0; i < optionArray.size(); ++i) {
          auto[tableIndex, _] = BaseField::convertIndex(*optionArray[i].field);
          int compareResult = compareFunctions[i](a[tableIndex], b[tableIndex]);
          if (compareResult < 0) {
            return optionArray[i].order == SortOrder::Ascending;
          } else if (compareResult > 0) {
            return optionArray[i].order == SortOrder::Descending;
          }
        }
        return false;
      };

  std::sort(data.begin() + 1,
            data.end() - 1,
            compareFunction);

  currentRecord = 0;
}

// TODO: implement more filter functions
std::shared_ptr<DataSets::ViewDataSet> DataSets::MemoryViewDataSet::filter(
    const DataSets::FilterOptions &options) {
  auto fieldNames = getFieldNames();
  std::vector<ValueType> fieldTypes;
  std::vector<std::pair<int, int>> fieldIndices;
  for (const auto &field : fields) {
    fieldTypes.emplace_back(field->getFieldType());
    fieldIndices.emplace_back(0, field->getIndex());
  }
  auto resultView = std::make_shared<MemoryViewDataSet>(getName() + "_filtered",
                                                        fieldNames,
                                                        fieldTypes,
                                                        fieldIndices);
  resultView->data.emplace_back();

  for (const auto &iter : data) {
    if (iter.empty()) {
      continue;
    }

    bool valid = true;

    for (const auto &filter : options.options) {
      if (!valid) {
        break;
      }

      auto[tableIndex, columnIndex] = BaseField::convertIndex(*filter.field);

      auto cell = (*iter[tableIndex])[columnIndex];

      if (filter.field->getFieldType() == ValueType::String) {
        std::string toCompare(cell._string);
        for (const auto &search : filter.searchData) {
          switch (filter.filterOption) {
            case FilterOption::Equals:
              valid = Utilities::compareString(toCompare,
                                               search._string) == 0;
              break;
            case FilterOption::StartsWith:
              valid = std::strncmp(toCompare.c_str(),
                                   search._string,
                                   strlen(search._string)) == 0;
              break;
            case FilterOption::Contains:
              valid = toCompare.find(search._string)
                  != std::string::npos;
              break;
            case FilterOption::EndsWith:
              valid = Utilities::endsWith(toCompare,
                                          search._string);
              break;
            case FilterOption::NotContains:
              valid = toCompare.find(search._string)
                  == std::string::npos;
              break;
            case FilterOption::NotStartsWith:
              valid = std::strncmp(toCompare.c_str(),
                                   search._string,
                                   strlen(search._string)) != 0;
              break;
            case FilterOption::NotEndsWith:
              valid = !Utilities::endsWith(toCompare,
                                           search._string);
              break;
          }
          if (valid) {
            break;
          }
        }

      } else if (filter.field->getFieldType() == ValueType::Integer) {
        auto toCompare = cell._integer;
        for (const auto &search : filter.searchData) {
          valid = Utilities::compareInt(toCompare, search._integer) == 0;
          if (valid) {
            break;
          }
        }
      } else if (filter.field->getFieldType() == ValueType::Double) {
        auto toCompare = cell._double;
        for (const auto &search : filter.searchData) {
          valid = Utilities::compareDouble(toCompare, search._double) == 0;
          if (valid) {
            break;
          }
        }
      } else if (filter.field->getFieldType() == ValueType::Currency) {
        auto toCompare = cell._currency;
        for (const auto &search : filter.searchData) {
          valid =
              Utilities::compareCurrency(*toCompare, *search._currency) == 0;
          if (valid) {
            break;
          }
        }
      } else if (filter.field->getFieldType() == ValueType::DateTime) {
        auto toCompare = cell._dateTime;
        for (const auto &search : filter.searchData) {
          valid =
              Utilities::compareDateTime(*toCompare, *search._dateTime) == 0;
          if (valid) {
            break;
          }
        }
      }
    }

    if (valid) {
      resultView->data.emplace_back(std::vector<DataSetRow *>{iter});
    }
  }
  resultView->data.emplace_back();
  return resultView;
}

bool DataSets::MemoryViewDataSet::findFirst(DataSets::FilterItem &item) {
  // TODO
}

void DataSets::MemoryViewDataSet::resetBegin() {
  currentRecord = 0;
}

void DataSets::MemoryViewDataSet::resetEnd() {
  currentRecord = data.size() - 1;
}

void DataSets::MemoryViewDataSet::setData(void *data,
                                          gsl::index index,
                                          ValueType type) {
  throw NotImplementedException();
}

void DataSets::MemoryViewDataSet::setFieldValues(gsl::index index) {
  for (gsl::index i = 0; i < fields.size(); i++) {
    auto[tableIndex, columnIndex] = BaseField::convertIndex(*fields[i]);

    auto cell = (*data[currentRecord][tableIndex])[columnIndex];
    switch (fields[i]->getFieldType()) {
      case ValueType::Integer:
        setFieldData(fields[i].get(),
                     &cell._integer);
        break;
      case ValueType::Double:
        setFieldData(fields[i].get(),
                     &cell._double);
        break;
      case ValueType::String:
        setFieldData(fields[i].get(),
                     cell._string);
        break;
      case ValueType::Currency:
        setFieldData(fields[i].get(),
                     cell._currency);
        break;
      case ValueType::DateTime:
        setFieldData(fields[i].get(),
                     cell._dateTime);
        break;
      default:throw IllegalStateException("Internal error.");
    }
  }
}

std::vector<std::vector<DataSetRow *>> *DataSets::MemoryViewDataSet::rawData() {
  return &data;
}

DataSets::MemoryViewDataSet::iterator DataSets::MemoryViewDataSet::begin() {
  return iterator(this, 1);
}

DataSets::MemoryViewDataSet::iterator DataSets::MemoryViewDataSet::end() {
  return iterator(this, data.size() - 1);
}
void DataSets::MemoryViewDataSet::createNullRows(const std::vector<std::pair<int,
                                                                             int>> &fieldIndices,
                                                 const std::vector<ValueType> &fieldTypes) {
  static gsl::zstring<> nullStr = "null";
  int last = fieldIndices[0].first;
  std::vector<DataContainer> nullData;
  for (gsl::index i = 0; i < fieldIndices.size(); ++i) {
    if (fieldIndices[i].first == last) {
      switch (fieldTypes[i]) {
        case ValueType::Integer:nullData.emplace_back(DataContainer{._integer = 0});
          break;
        case ValueType::Double:nullData.emplace_back(DataContainer{._double = 0.0});
          break;
        case ValueType::String:nullData.emplace_back(DataContainer{._string = nullStr});
          break;
        case ValueType::Currency:
          nullData.emplace_back(DataContainer{._currency = new Currency(0)});
          break;
        case ValueType::DateTime:nullData.emplace_back(DataContainer{._dateTime = new DateTime()});
          break;
      }
    } else {
      nullRecords.emplace_back(new DataSetRow(nullData));
      nullData.clear();
      last = fieldIndices[i].first;
      --i;
    }
  }
  nullRecords.emplace_back(new DataSetRow(nullData));
}

DataSetRow *DataSets::MemoryViewDataSet::getNullRow(gsl::index tableIndex) {
  return nullRecords[tableIndex];
}

gsl::index DataSets::MemoryViewDataSet::getTableCount() {
  return nullRecords.size();
}

void DataSets::MemoryViewDataSet::setAllowedFields(const std::vector<std::string> &fieldNames) {
  allowedFields.clear();
  std::transform(fieldNames.begin(),
                 fieldNames.end(),
                 std::back_inserter(allowedFields),
                 [this](std::string_view fieldName) {
                   return fieldByName(fieldName);
                 });
}
void DataSets::MemoryViewDataSet::addParent(std::shared_ptr<MemoryDataSet> &parent) {
  parents.emplace_back(parent);
}
void DataSets::MemoryViewDataSet::addParents(const std::vector<std::shared_ptr<
    DataSets::MemoryDataSet>> &parents) {
  std::copy(parents.begin(), parents.end(), std::back_inserter(this->parents));
}
const std::vector<std::shared_ptr<DataSets::MemoryDataSet>> &DataSets::MemoryViewDataSet::getParents() const {
  return parents;
}
