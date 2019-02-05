//
// Created by Petr Flajsingr on 25/08/2018.
//

#include <MemoryDataSet.h>
#include <FieldFactory.h>

void DataSets::MemoryDataSet::open(DataProviders::BaseDataProvider &dataProvider,
                                   const std::vector<ValueType> &fieldTypes) {
  if (isOpen) {
    throw IllegalStateException("Dataset is already open.");
  }
  createFields(dataProvider.getHeader(), fieldTypes);
  data.emplace_back(stopItem);
  data.emplace_back(stopItem);
  loadData(dataProvider);
  data.shrink_to_fit();

  isOpen = true;
  setFieldValues(getFirst(), true);
}

void DataSets::MemoryDataSet::openEmpty(const std::vector<std::string> &fieldNames,
                                        const std::vector<ValueType> &fieldTypes) {
  if (isOpen) {
    throw IllegalStateException("Dataset is already open.");
  }
  createFields(fieldNames, fieldTypes);
  data.emplace_back(stopItem);
  data.emplace_back(stopItem);
  isOpen = true;
}

void DataSets::MemoryDataSet::loadData(DataProviders::BaseDataProvider &dataProvider) {
  data.pop_back();
  while (dataProvider.next()) {
    addRecord(dataProvider);
  }
  data.emplace_back(stopItem);
}

void DataSets::MemoryDataSet::createFields(std::vector<std::string> columns,
                                           std::vector<ValueType> types) {
  Expects(columns.size() == types.size());
  columnCount = fields.size();
  for (gsl::index i = 0; i < columns.size(); ++i) {
    fields.emplace_back(FieldFactory::Get().CreateField(
        columns[i],
        i,
        types[i],
        this));
  }
}

void DataSets::MemoryDataSet::addRecord(DataProviders::BaseDataProvider &dataProvider) {
  auto record = dataProvider.getRow();
  data.emplace_back(DataSetRow{true, {}});

  data.back().cells.reserve(columnCount);
  for (gsl::index i = 0; i < record.size(); ++i) {
    switch (fields[i]->getFieldType()) {
      case ValueType::Integer:
        data.back().cells.emplace_back(DataContainer{._integer = Utilities::stringToInt(
            record[i])});
        break;
      case ValueType::Double:
        data.back().cells.emplace_back(DataContainer{._double = Utilities::stringToDouble(
            record[i])});
        break;
      case ValueType::String:
        data.back().cells.emplace_back(DataContainer{._string = strdup(record[i].c_str())});
        break;
      case ValueType::Currency:
        data.back().cells.emplace_back(DataContainer{._currency = new Currency(
            record[i])});
        break;
      case ValueType::DateTime:
        data.back().cells.emplace_back(DataContainer{._dateTime = new DateTime(
            record[i])});
        break;
      default:
        throw IllegalStateException(
            "Internal error. DataSets::MemoryDataSet::addRecord");
    }
  }
}

void DataSets::MemoryDataSet::close() {
  isOpen = false;
  if (!data.empty()) {
    for (auto level1 : data) {
      gsl::index iter = 0;
      for (auto level2 : level1.cells) {
        if (fields[iter]->getFieldType() == ValueType::String) {
          delete level2._string;
        } else if (fields[iter]->getFieldType() == ValueType::Currency) {
          delete level2._currency;
        } else if (fields[iter]->getFieldType() == ValueType::DateTime) {
          delete level2._dateTime;
        }
        iter++;
      }
      level1.cells.clear();
    }
    data.clear();
  }
}

// TODO: rework
bool DataSets::MemoryDataSet::setFieldValues(gsl::index index,
                                             bool searchForward) {
  gsl::index iter = index;
  if (dataValidityChanged) {
    bool found = false;

    if (searchForward) {
      for (iter = index; iter < data.size(); iter++) {
        if (data[iter].valid) {
          found = true;
          break;
        }
      }
    } else {
      for (iter = index; iter >= 0; iter--) {
        if (data[iter].valid) {
          found = true;
          break;
        }
      }
    }
    if (!found) {
      if (searchForward) {
        currentRecord = getLast();
      } else {
        currentRecord = getFirst();
      }
      return false;
    }
    currentRecord = iter;
  }

  for (size_t i = 0; i < fields.size(); i++) {
    switch (fields[i]->getFieldType()) {
      case ValueType::Integer:
        setFieldData(fields[i].get(),
                     &data[iter].cells[i]._integer);
        break;
      case ValueType::Double:
        setFieldData(fields[i].get(),
                     &data[iter].cells[i]._double);
        break;
      case ValueType::String:
        setFieldData(fields[i].get(),
                     data[iter].cells[i]._string);
        break;
      case ValueType::Currency:
        setFieldData(fields[i].get(),
                     data[iter].cells[i]._currency);
        break;
      case ValueType::DateTime:
        setFieldData(fields[i].get(),
                     data[iter].cells[i]._dateTime);
        break;
      default:throw IllegalStateException("Internal error.");
    }
  }
  return true;
}

void DataSets::MemoryDataSet::first() {
  currentRecord = getFirst();
  setFieldValues(currentRecord, true);
}

void DataSets::MemoryDataSet::last() {
  currentRecord = getLast();
  setFieldValues(currentRecord, false);
}

bool DataSets::MemoryDataSet::next() {
  currentRecord++;
  if (currentRecord == data.size() - 1) {
    return false;
  }
  setFieldValues(currentRecord, true);
  return true;
}

bool DataSets::MemoryDataSet::previous() {
  currentRecord--;
  if (currentRecord == 0) {
    return false;
  }
  setFieldValues(currentRecord, false);
  return true;
}

void DataSets::MemoryDataSet::sort(SortOptions &options) {
  uint64_t fieldCount = fields.size();
  bool isInRange = std::all_of(options.options.begin(),
                               options.options.end(),
                               [fieldCount](SortItem &item) {
                                 return item.fieldIndex < fieldCount;
                               });
  if (!isInRange) {
    throw InvalidArgumentException("Field index is out of bounds");
  }

  std::vector<std::function<int8_t(const DataSetRow &, const DataSetRow &)>>
      compareFunctions;

  std::transform(options.options.begin(),
                 options.options.end(),
                 std::back_inserter(compareFunctions),
                 [this](const SortItem &option) {
                   return fields[option.fieldIndex]->getCompareFunction();
                 });

  auto optionArray = options.options;
  auto compareFunction =
      [&optionArray, &compareFunctions](const DataSetRow &a,
                                        const DataSetRow &b) {
        for (gsl::index i = 0; i < optionArray.size(); ++i) {
          int compareResult = compareFunctions[i](a, b);
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

// TODO: rework
void DataSets::MemoryDataSet::filter(const FilterOptions &options) {
  if (options.options.empty()) {
    dataValidityChanged = false;
    for (auto &iter : data) {
      iter.valid = true;
    }
    first();
    return;
  }
  dataValidityChanged = true;

  gsl::index i = 0;
  for (const auto &iter : data) {
    if (iter.cells.empty()) {
      continue;
    }
    bool valid = true;

    gsl::index optionCounter = 0;
    for (const auto &filter : options.options) {
      if (!valid) {
        break;
      }

      if (filter.type == ValueType::String) {
        std::string toCompare = iter.cells[filter.fieldIndex]._string;
        for (const auto &search : filter.searchData) {
          switch (filter.filterOption) {
            case FilterOption::Equals:
              valid = std::strcmp(toCompare.c_str(),
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
        }
      } else if (filter.type == ValueType::Integer) {
        auto toCompare = iter.cells[filter.fieldIndex]._integer;
        for (const auto &search : filter.searchData) {
          valid = Utilities::compareInt(toCompare, search._integer) == 0;
        }
      } else if (filter.type == ValueType::Double) {
        auto toCompare = iter.cells[filter.fieldIndex]._double;
        for (const auto &search : filter.searchData) {
          valid = Utilities::compareDouble(toCompare, search._integer) == 0;
        }
      } else if (filter.type == ValueType::Currency) {
        auto toCompare = iter.cells[filter.fieldIndex]._currency;
        for (const auto &search : filter.searchData) {
          valid =
              Utilities::compareCurrency(*toCompare, *search._currency) == 0;
        }
      } else if (filter.type == ValueType::DateTime) {
        auto toCompare = iter.cells[filter.fieldIndex]._dateTime;
        for (const auto &search : filter.searchData) {
          valid =
              Utilities::compareDateTime(*toCompare, *search._dateTime) == 0;
        }
      }

      optionCounter++;
    }

    data[i].valid = valid;
    i++;
  }

  first();
}

DataSets::BaseField *DataSets::MemoryDataSet::fieldByName(
    std::string_view name) const {
  for (auto &field : fields) {
    if (Utilities::compareString(field->getFieldName(), name) == 0) {
      return field.get();
    }
  }
  std::string errMsg = "Field named \"" + std::string(name)
      + "\" not found. DataSets::MemoryDataSet::fieldByName";
  throw InvalidArgumentException(errMsg.c_str());
}

DataSets::BaseField *DataSets::MemoryDataSet::fieldByIndex(gsl::index index) const {
  return fields.at(index).get();
}

bool DataSets::MemoryDataSet::isLast() const {
  return currentRecord >= getLast();
}

DataSets::MemoryDataSet::~MemoryDataSet() {
  close();
}

void DataSets::MemoryDataSet::setData(void *data,
                                      gsl::index index,
                                      ValueType type) {
  switch (type) {
    case ValueType::Integer:
      this->data[currentRecord].cells[index]._integer
          = *reinterpret_cast<int *>(data);
      break;
    case ValueType::Double:
      this->data[currentRecord].cells[index]._double
          = *reinterpret_cast<int *>(data);
      break;
    case ValueType::String:delete[] this->data[currentRecord].cells[index]._string;
      this->data[currentRecord].cells[index]._string
          = reinterpret_cast<gsl::zstring<>>(data);
      break;
    case ValueType::Currency:
      *(this->data[currentRecord].cells[index]._currency)
          = *(reinterpret_cast<Currency *>(data));
      break;
    case ValueType::DateTime:
      *(this->data[currentRecord].cells[index]._dateTime)
          = *(reinterpret_cast<DateTime *>(data));
      break;
    default:throw IllegalStateException("Invalid value type.");
  }
}

void DataSets::MemoryDataSet::append() {
  data.pop_back();
  for (auto &field : fields) {
    switch (field->getFieldType()) {
      case ValueType::Integer:data.back().cells.emplace_back(DataContainer{._integer = 0});
        break;
      case ValueType::Double:data.back().cells.emplace_back(DataContainer{._double = 0});
        break;
      case ValueType::String:data.back().cells.emplace_back(DataContainer{._string = nullptr});
        break;
      case ValueType::Currency:data.back().cells.emplace_back(DataContainer{._currency = new Currency()});
        break;
      case ValueType::DateTime:data.back().cells.emplace_back(DataContainer{._dateTime = new DateTime()});
        break;
      default:
        throw IllegalStateException(
            "Internal error DataSets::MemoryDataSet::append().");
    }
  }
  last();
  data.emplace_back(stopItem);
}

void DataSets::MemoryDataSet::append(
    DataProviders::BaseDataProvider &dataProvider) {
  if (!isOpen) {
    throw IllegalStateException("Dataset is not open.");
  }
  loadData(dataProvider);
  data.shrink_to_fit();
}

DataSets::MemoryDataSet::MemoryDataSet(std::string_view dataSetName)
    : BaseDataSet(dataSetName) {}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
// TODO: predelat na iterator a std::lower_bound
bool DataSets::MemoryDataSet::findFirst(FilterItem &item) {
  auto field = fields[item.fieldIndex];
  unsigned long min = getFirst(), max = getLast();
  bool breakLoop = false;

  do {
    int8_t comparisonResult;
    switch (field->getFieldType()) {
      case ValueType::String:
        comparisonResult = Utilities::compareString(
            field->getAsString(),
            item.searchData[0]._string);
        break;
      case ValueType::Integer:
        comparisonResult = Utilities::compareInt(
            reinterpret_cast<IntegerField *>(field.get())->getAsInteger(),
            item.searchData[0]._integer);
        break;
      case ValueType::Double:
        comparisonResult = Utilities::compareDouble(
            reinterpret_cast<DoubleField *>(field.get())->getAsDouble(),
            item.searchData[0]._double);
        break;
      case ValueType::Currency: {
        auto cur =
            reinterpret_cast<CurrencyField *>(field.get())->getAsCurrency();
        comparisonResult = Utilities::compareCurrency(
            cur,
            *item.searchData[0]._currency);
      }
        break;
      case ValueType::DateTime:
        auto dateTime =
            reinterpret_cast<DateTimeField *>(field.get())->getAsDateTime();
        comparisonResult =
            Utilities::compareDateTime(dateTime, *item.searchData[0]._dateTime);
        break;
    }
    switch (comparisonResult) {
      case 0:return true;
      case -1:min = currentRecord;
        currentRecord = (min + max) >> 1;
        setFieldValues(currentRecord, true);
        break;
      case 1:max = currentRecord;
        currentRecord = (min + max) >> 1;
        setFieldValues(currentRecord, true);
        break;
      default:
        throw IllegalStateException(
            "Internal error DataSets::MemoryDataSet::findFirst");
    }
    if (max - min < 2) {
      if (breakLoop) {
        return false;
      }
      breakLoop = true;
    }
  } while (currentRecord > 0 && currentRecord < data.size());

  if (currentRecord == 0) {
    int8_t comparisonResult;
    switch (field->getFieldType()) {
      case ValueType::String:
        comparisonResult = Utilities::compareString(
            field->getAsString(),
            item.searchData[0]._string);
        break;
      case ValueType::Integer:
        comparisonResult = Utilities::compareInt(
            reinterpret_cast<IntegerField *>(field.get())->getAsInteger(),
            item.searchData[0]._integer);
        break;
      case ValueType::Double:
        comparisonResult = Utilities::compareDouble(
            reinterpret_cast<DoubleField *>(field.get())->getAsDouble(),
            item.searchData[0]._double);
        break;
      case ValueType::Currency: {
        Currency
            cur =
            reinterpret_cast<CurrencyField *>(field.get())->getAsCurrency();
        comparisonResult = Utilities::compareCurrency(
            cur,
            *item.searchData[0]._currency);
      }
        break;
      case ValueType::DateTime:
        DateTime dateTime =
            reinterpret_cast<DateTimeField *>(field.get())->getAsDateTime();
        comparisonResult = Utilities::compareDateTime(
            dateTime,
            *item.searchData[0]._dateTime);
        break;
    }

    if (comparisonResult == 0) {
      return true;
    }
  }
  return false;
}

std::vector<DataSets::BaseField *> DataSets::MemoryDataSet::getFields() const {
  std::vector<BaseField *> result;
  for (const auto &field : fields) {
    result.emplace_back(field.get());
  }
  return result;
}

std::vector<std::string> DataSets::MemoryDataSet::getFieldNames() const {
  std::vector<std::string> result;
  std::transform(fields.begin(),
                 fields.end(),
                 result.begin(),
                 [](const std::shared_ptr<BaseField> &field) {
                   return field->getFieldName();
                 });
  return result;
}

bool DataSets::MemoryDataSet::isFirst() const {
  return currentRecord == 0;
}

gsl::index DataSets::MemoryDataSet::getFirst() const {
  return 1;
}

gsl::index DataSets::MemoryDataSet::getLast() const {
  return data.size() - 2;
}

bool DataSets::MemoryDataSet::isBegin() const {
  return currentRecord == 0;
}

bool DataSets::MemoryDataSet::isEnd() const {
  return currentRecord == data.size() - 1;
}

#pragma clang diagnostic pop
