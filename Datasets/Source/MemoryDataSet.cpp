//
// Created by Petr Flajsingr on 25/08/2018.
//

#include <MemoryDataSet.h>

void DataSets::MemoryDataSet::setDataProvider(
    DataProviders::BaseDataProvider *provider) {
  dataProvider = provider;
}

void DataSets::MemoryDataSet::open() {
  if (isOpen) {
    throw IllegalStateException("Dataset is already open.");
  }

  if (dataProvider == nullptr) {
    throw IllegalStateException("Data provider has not been set.");
  }

  loadData();
  data.shrink_to_fit();

  isOpen = true;

  setFieldValues(0, true);
}

void DataSets::MemoryDataSet::openEmpty() {
  if (isOpen) {
    throw IllegalStateException("Dataset is already open.");
  }

  isOpen = true;
}

void DataSets::MemoryDataSet::loadData() {
  while (!dataProvider->eof()) {
    addRecord();

    dataProvider->next();
  }
}

void DataSets::MemoryDataSet::createFields(std::vector<std::string> columns,
                                           std::vector<ValueType> types) {
  if (columns.size() != types.size()) {
    std::string errMsg =
        "Amount of columns and types must match. Data set: " + this->getName();
    throw InvalidArgumentException(errMsg.c_str());
  }
  size_t iter = 0;
  for (const auto &col : columns) {
    BaseField *newField = nullptr;
    switch (types[iter]) {
      case IntegerValue: {
        newField = new IntegerField(col, this, iter);
        break;
      }
      case DoubleValue: {
        newField = new DoubleField(col, this, iter);
        break;
      }
      case StringValue: {
        newField = new StringField(col, this, iter);
        break;
      }
      case CurrencyValue: {
        newField = new CurrencyField(col, this, iter);
        break;
      }
      case DateTimeValue: {
        newField = new DateTimeField(col, this, iter);
      }
      default:throw IllegalStateException("Internal error.");
    }
    fields.emplace_back(newField);
    iter++;
  }
}

void DataSets::MemoryDataSet::addRecord() {
  auto record = dataProvider->getRow();

  auto newRecord = new DataSetRowCells();
  newRecord->reserve(columnCount);
  size_t iter = 0;
  for (const auto &part : record) {
    switch (fields[iter]->getFieldType()) {
      case IntegerValue:
        newRecord->emplace_back(new DataContainer({._integer = Utilities::stringToInt(
            part)}));
        break;
      case DoubleValue:
        newRecord->emplace_back(new DataContainer({._double = Utilities::stringToDouble(
            part)}));
        break;
      case StringValue:
        newRecord->emplace_back(new DataContainer({._string = strdup(part.c_str())}));
        break;
      case CurrencyValue:
        newRecord->emplace_back(new DataContainer({._currency = new Currency(
            part)}));
        break;
      case DateTimeValue:
        newRecord->emplace_back(new DataContainer({._dateTime = new DateTime(
            DateTime_s)}));
        break;
      default:throw IllegalStateException("Internal error.");
    }
    iter++;
  }

  data.emplace_back(new DataSetRow{true, newRecord});
}

void DataSets::MemoryDataSet::close() {
  dataProvider = nullptr;

  isOpen = false;

  if (!data.empty()) {
    for (auto level1 : data) {
      size_t iter = 0;
      for (auto level2 : *level1->cells) {
        if (fields[iter]->getFieldType() == StringValue)
          delete level2->_string;
        else if (fields[iter]->getFieldType() == CurrencyValue)
          delete level2->_currency;
        else if (fields[iter]->getFieldType() == DateTimeValue)
          delete level2->_dateTime;
        delete level2;
        iter++;
      }
      level1->cells->clear();
      delete level1->cells;
    }
    data.clear();
  }
}

bool DataSets::MemoryDataSet::setFieldValues(uint64_t index,
                                             bool searchForward) {
  int64_t iter = index;
  if (dataValidityChanged) {
    bool found = false;

    if (searchForward) {
      for (iter = index; iter < data.size(); iter++) {
        if (data[iter]->valid) {
          found = true;
          break;
        }
      }
    } else {
      for (iter = index; iter >= 0; iter--) {
        if (data[iter]->valid) {
          found = true;
          break;
        }
      }
    }
    if (!found) {
      if (searchForward) {
        currentRecord = data.size();
      } else {
        currentRecord = 0;
      }
      return false;
    }
    currentRecord = static_cast<uint64_t>(iter);
  }

  DataSetRowCells *value = data[iter]->cells;

  for (size_t i = 0; i < fields.size(); i++) {
    switch (fields[i]->getFieldType()) {
      case IntegerValue:setFieldData(fields[i], &(*value)[i]->_integer);
        break;
      case DoubleValue:setFieldData(fields[i], &(*value)[i]->_double);
        break;
      case StringValue:setFieldData(fields[i], (*value)[i]->_string);
        break;
      case CurrencyValue:setFieldData(fields[i], (*value)[i]->_currency);
        break;
      case DateTimeValue:setFieldData(fields[i], (*value)[i]->_dateTime);
        break;
      default:throw IllegalStateException("Internal error.");
    }
  }

  return true;
}

void DataSets::MemoryDataSet::first() {
  currentRecord = 0;
  setFieldValues(0, true);
}

void DataSets::MemoryDataSet::last() {
  currentRecord = data.size() - 1;
  setFieldValues(data.size() - 1, false);
}

void DataSets::MemoryDataSet::next() {
  currentRecord++;

  if (currentRecord > data.size()) {
    currentRecord = data.size();
  }
  if (!eof()) {
    setFieldValues(currentRecord, true);
  }
}

void DataSets::MemoryDataSet::previous() {
  if (currentRecord == 0) {
    return;
  }
  currentRecord--;
  setFieldValues(currentRecord, false);
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

  std::vector<std::function<int8_t(DataSetRow *, DataSetRow *)>>
      compareFunctions;

  for (auto &option : options.options) {
    compareFunctions.emplace_back(
        fields[option.fieldIndex]->getCompareFunction());
  }

  auto optionArray = options.options;
  auto compareFunction =
      [&optionArray, &compareFunctions](DataSetRow *a, DataSetRow *b) {
        for (uint8_t i = 0; i < optionArray.size(); ++i) {
          int compareResult = compareFunctions[i](a, b);
          if (compareResult < 0) {
            return optionArray[i].order == SortOrder::Ascending;
          } else if (compareResult > 0) {
            return optionArray[i].order == SortOrder::Descending;
          }
        }
        return false;
      };

  std::sort(data.begin(),
            data.end(),
            compareFunction);

  first();
}

void DataSets::MemoryDataSet::filter(const FilterOptions &options) {
  if (options.options.empty()) {
    dataValidityChanged = false;

    for (auto &iter : data) {
      iter->valid = true;
    }

    first();

    return;
  }

  dataValidityChanged = true;

  size_t i = 0;
  std::string toCompare;
  for (auto iter : data) {
    bool valid = true;

    size_t optionCounter = 0;
    for (auto filter : options.options) {
      if (!valid) {
        break;
      }

      if (filter.type == ValueType::StringValue) {
        std::string toCompare = (*iter->cells)[filter.fieldIndex]->_string;
        for (const auto &search : filter.searchData) {
          switch (filter.filterOption) {
            case EQUALS:
              valid = std::strcmp(toCompare.c_str(), search._string) == 0;
              break;
            case STARTS_WITH:
              valid = std::strncmp(toCompare.c_str(),
                                   search._string,
                                   strlen(search._string)) == 0;
              break;
            case CONTAINS:
              valid = toCompare.find(search._string)
                  != std::string::npos;
              break;
            case ENDS_WITH:
              valid = Utilities::endsWith(toCompare, search._string);
              break;
            case NOT_CONTAINS:
              valid = toCompare.find(search._string)
                  == std::string::npos;
              break;
            case NOT_STARTS_WITH:
              valid = std::strncmp(toCompare.c_str(),
                                   search._string,
                                   strlen(search._string)) != 0;
              break;
            case NOT_ENDS_WITH:
              valid = !Utilities::endsWith(toCompare,
                                           search._string);
              break;
          }
        }
      } else if (filter.type == ValueType::IntegerValue) {
        int toCompare = (*iter->cells)[filter.fieldIndex]->_integer;
        for (const auto &search : filter.searchData) {
          valid = Utilities::compareInt(toCompare, search._integer) == 0;
        }
      } else if (filter.type == ValueType::DoubleValue) {
        double toCompare = (*iter->cells)[filter.fieldIndex]->_double;
        for (const auto &search : filter.searchData) {
          valid = Utilities::compareDouble(toCompare, search._integer) == 0;
        }
      } else if (filter.type == ValueType::CurrencyValue) {
        Currency *toCompare = (*iter->cells)[filter.fieldIndex]->_currency;
        for (const auto &search : filter.searchData) {
          valid =
              Utilities::compareCurrency(*toCompare, *search._currency) == 0;
        }
      } else if (filter.type == ValueType::DateTimeValue) {
        DateTime *toCompare = (*iter->cells)[filter.fieldIndex]->_dateTime;
        for (const auto &search : filter.searchData) {
          valid =
              Utilities::compareDateTime(*toCompare, *search._dateTime) == 0;
        }
      }

      optionCounter++;
    }

    data[i]->valid = valid;
    i++;
  }

  first();
}

DataSets::BaseField *DataSets::MemoryDataSet::fieldByName(
    const std::string &name) {
  for (auto &field : fields) {
    if (Utilities::compareString(field->getFieldName(), name) == 0) {
      return field;
    }
  }
  std::string errMsg = "Field named \"" + name
      + "\" not found. DataSets::MemoryDataSet::fieldByName";
  throw InvalidArgumentException(errMsg.c_str());
}

DataSets::BaseField *DataSets::MemoryDataSet::fieldByIndex(uint64_t index) {
  return fields.at(index);
}

bool DataSets::MemoryDataSet::eof() {
  return currentRecord >= data.size();
}

DataSets::MemoryDataSet::~MemoryDataSet() {
  close();
}

void DataSets::MemoryDataSet::setFieldTypes(std::vector<ValueType> types) {
  createFields(dataProvider->getHeader(), types);
  columnCount = fields.size();
}

void DataSets::MemoryDataSet::setFieldTypes(std::vector<std::string> fieldNames,
                                            std::vector<ValueType> types) {
  createFields(fieldNames, types);
  columnCount = fields.size();
}

void DataSets::MemoryDataSet::setData(void *data,
                                      uint64_t index,
                                      ValueType type) {
  switch (type) {
    case IntegerValue:
      (*this->data[currentRecord]->cells)[index]->_integer
          = *reinterpret_cast<int *>(data);
      break;
    case DoubleValue:
      (*this->data[currentRecord]->cells)[index]->_double
          = *reinterpret_cast<int *>(data);
      break;
    case StringValue:delete[] (*this->data[currentRecord]->cells)[index]->_string;
      (*this->data[currentRecord]->cells)[index]->_string
          = reinterpret_cast<char *>(data);
      break;
    case CurrencyValue:
      *((*this->data[currentRecord]->cells)[index]->_currency)
          = *(reinterpret_cast<Currency *>(data));
      break;
    case DateTimeValue:
      *((*this->data[currentRecord]->cells)[index]->_dateTime)
          = *(reinterpret_cast<DateTime *>(data));
      break;
    default:throw IllegalStateException("Invalid value type.");
  }

}

void DataSets::MemoryDataSet::append() {
  auto newRecord = new DataSetRowCells();
  for (size_t i = 0; i < fields.size(); ++i) {
    auto dataContainer = new DataContainer();

    switch (fields[i]->getFieldType()) {
      case IntegerValue:dataContainer->_integer = 0;
        break;
      case DoubleValue:dataContainer->_double = 0;
        break;
      case StringValue:dataContainer->_string = nullptr;
        break;
      case CurrencyValue:dataContainer->_currency = new Currency();
        break;
      case DateTimeValue:dataContainer->_dateTime = new DateTime(DateTime_s);
        break;
      default:
        throw IllegalStateException(
            "Internal error DataSets::MemoryDataSet::append().");
    }
    newRecord->emplace_back(dataContainer);
  }

  data.emplace_back(new DataSetRow{true, newRecord});
  last();
}

void DataSets::MemoryDataSet::appendDataProvider(
    DataProviders::BaseDataProvider *provider) {
  if (!isOpen) {
    throw IllegalStateException("Dataset is not open.");
  }

  if (provider == nullptr) {
    throw InvalidArgumentException("Data provider has not been set.");
  }

  dataProvider = provider;

  loadData();
  data.shrink_to_fit();
}

DataSets::MemoryDataSet::MemoryDataSet(const std::string &dataSetName)
    : BaseDataSet(dataSetName) {}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
bool DataSets::MemoryDataSet::findFirst(FilterItem &item) {
  auto field = fields[item.fieldIndex];

  unsigned long min = 0, max = data.size();

  bool breakLoop = false;

  do {
    int8_t comparisonResult;
    switch (field->getFieldType()) {
      case StringValue:
        comparisonResult = Utilities::compareString(
            field->getAsString(),
            item.searchData[0]._string);
        break;
      case IntegerValue:
        comparisonResult = Utilities::compareInt(
            reinterpret_cast<IntegerField *>(field)->getAsInteger(),
            item.searchData[0]._integer);
        break;
      case DoubleValue:
        comparisonResult = Utilities::compareDouble(
            reinterpret_cast<DoubleField *>(field)->getAsDouble(),
            item.searchData[0]._double);
        break;
      case CurrencyValue: {
        auto cur = reinterpret_cast<CurrencyField *>(field)->getAsCurrency();
        comparisonResult = Utilities::compareCurrency(
            cur,
            *item.searchData[0]._currency);
      }
        break;
      case DateTimeValue:
        auto dateTime =
            reinterpret_cast<DateTimeField *>(field)->getAsDateTime();
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
      case StringValue:
        comparisonResult = Utilities::compareString(
            field->getAsString().c_str(),
            item.searchData[0]._string);
        break;
      case IntegerValue:
        comparisonResult = Utilities::compareInt(
            reinterpret_cast<IntegerField *>(field)->getAsInteger(),
            item.searchData[0]._integer);
        break;
      case DoubleValue:
        comparisonResult = Utilities::compareDouble(
            reinterpret_cast<DoubleField *>(field)->getAsDouble(),
            item.searchData[0]._double);
        break;
      case CurrencyValue: {
        Currency
            cur = reinterpret_cast<CurrencyField *>(field)->getAsCurrency();
        comparisonResult = Utilities::compareCurrency(
            cur,
            *item.searchData[0]._currency);
      }
        break;
      case DateTimeValue:
        DateTime dateTime =
            reinterpret_cast<DateTimeField *>(field)->getAsDateTime();
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

std::vector<DataSets::BaseField *> DataSets::MemoryDataSet::getFields() {
  return fields;
}

std::vector<std::string> DataSets::MemoryDataSet::getFieldNames() {
  std::vector<std::string> result;

  for (const auto &field : fields) {
    result.push_back(field->getFieldName());
  }

  return result;
}

#pragma clang diagnostic pop
