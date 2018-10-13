//
// Created by Petr Flajsingr on 25/08/2018.
//

#include "MemoryDataSet.h"

void DataSets::MemoryDataSet::setDataProvider(DataProviders::BaseDataProvider *provider) {
  this->dataProvider = provider;
}

void DataSets::MemoryDataSet::open() {
  if (isOpen) {
    throw IllegalStateException("Dataset is already open.");
  }

  if (this->dataProvider == nullptr) {
    throw IllegalStateException("Data provider has not been set.");
  }

  this->loadData();

  this->isOpen = true;

  setFieldValues(0, true);
}

void DataSets::MemoryDataSet::loadData() {
  while (!this->dataProvider->eof()) {
    this->addRecord();

    this->dataProvider->next();
  }
}

void DataSets::MemoryDataSet::createFields(std::vector<std::string> columns,
                                 std::vector<ValueType> types) {
  if (columns.size() != types.size()) {
    throw InvalidArgumentException(
        "Amount of columns and types must match.");
  }
  size_t iter = 0;
  for (const auto &col : columns) {
    BaseField *newField = nullptr;
    switch (types[iter]) {
      case INTEGER_VAL: {
        newField = new IntegerField(col, this, iter);
        break;
      }
      case DOUBLE_VAL: {
        newField = new DoubleField(col, this, iter);
        break;
      }
      case STRING_VAL: {
        newField = new StringField(col, this, iter);
        break;
      }
      default:throw IllegalStateException("Internal error.");
    }
    fields.push_back(newField);
    iter++;
  }
}

void DataSets::MemoryDataSet::addRecord() {
  auto record = this->dataProvider->getRow();

  auto newRecord = new DataSetRowCells();
  size_t iter = 0;
  for (const auto &part : record) {
    auto dataContainer = new DataContainer();
    switch (fields[iter]->getFieldType()) {
      case INTEGER_VAL:
        dataContainer->_integer = Utilities::stringToInt(part);
        break;
      case DOUBLE_VAL:
        dataContainer->_double = Utilities::stringToDouble(part);
        break;
      case STRING_VAL:
        dataContainer->_string = strdup(part.c_str());
        break;
      default:throw IllegalStateException("Internal error.");
    }
    iter++;

    newRecord->push_back(dataContainer);
  }

  this->data.push_back(new DataSetRow {true, newRecord});
}

void DataSets::MemoryDataSet::close() {
  this->dataProvider = nullptr;

  this->isOpen = false;

  if (!this->data.empty()) {
    for (auto level1 : this->data) {
      size_t iter = 0;
      for (auto level2 : *level1->cells) {
        if (this->fields[iter])
          delete level2;
        iter++;
      }
      level1->cells->clear();
      delete level1->cells;
    }
    this->data.clear();
  }
}

bool DataSets::MemoryDataSet::setFieldValues(uint64_t index,
    bool searchForward) {
  int64_t iter = index;
  if (dataValidityChanged) {
    bool found = false;

    if (searchForward) {
      for (iter = index; iter < this->data.size(); iter++) {
        if (this->data[iter]->valid) {
          found = true;
          break;
        }
      }
    } else {
      for (iter = index; iter >= 0; iter--) {
        if (this->data[iter]->valid) {
          found = true;
          break;
        }
      }
    }
    if (!found) {
      if (searchForward) {
        this->currentRecord = this->data.size();
      } else {
        this->currentRecord = 0;
      }
      return false;
    }
    this->currentRecord = static_cast<uint64_t>(iter);
  }

  DataSetRowCells *value = this->data[iter]->cells;

  for (size_t i = 0; i < fields.size(); i++) {
    switch (fields[i]->getFieldType()) {
      case INTEGER_VAL:
        this->setFieldData(fields[i], &(*value)[i]->_integer);
        break;
      case DOUBLE_VAL:
        this->setFieldData(fields[i], &(*value)[i]->_double);
        break;
      case STRING_VAL:
        this->setFieldData(fields[i], (*value)[i]->_string);
        break;
      default:throw IllegalStateException("Internal error.");
    }
  }

  return true;
}

void DataSets::MemoryDataSet::first() {
  this->currentRecord = 0;
  setFieldValues(0, true);
}

void DataSets::MemoryDataSet::last() {
  this->currentRecord = this->data.size() - 1;
  setFieldValues(this->data.size() - 1, false);
}

void DataSets::MemoryDataSet::next() {
  this->currentRecord++;
  if (!this->eof()) {
    setFieldValues(this->currentRecord, true);
  }
}

void DataSets::MemoryDataSet::previous() {
  if (this->currentRecord == 0) {
    return;
  }
  this->currentRecord--;
  setFieldValues(this->currentRecord, false);
}

void DataSets::MemoryDataSet::sort(uint64_t fieldIndex, SortOrder sortOrder) {
  if (fieldIndex >= this->getFieldNames().size()) {
    throw InvalidArgumentException("Field index is out of bounds");
  }

  if (sortOrder == ASCENDING) {
    std::sort(this->data.begin(),
              this->data.end(),
              [&fieldIndex, this](const DataSetRow *a,
                                  const DataSetRow *b) {
                switch (this->fields[fieldIndex]->getFieldType()) {
                  case INTEGER_VAL:
                    return (*a->cells)[fieldIndex]->_integer
                        < (*b->cells)[fieldIndex]->_integer;
                  case DOUBLE_VAL:
                    return (*a->cells)[fieldIndex]->_double
                        < (*b->cells)[fieldIndex]->_double;
                  case STRING_VAL:
                    return std::strcmp((*a->cells)[fieldIndex]->_string,
                                       (*b->cells)[fieldIndex]->_string) < 0;
                  default:throw IllegalStateException("Internal error.");
                }
              });
  } else {
    std::sort(this->data.begin(),
              this->data.end(),
              [&fieldIndex, this](const DataSetRow *a,
                                  const DataSetRow *b) {
                switch (this->fields[fieldIndex]->getFieldType()) {
                  case INTEGER_VAL:
                    return (*a->cells)[fieldIndex]->_integer
                        > (*b->cells)[fieldIndex]->_integer;
                  case DOUBLE_VAL:
                    return (*a->cells)[fieldIndex]->_double
                        > (*b->cells)[fieldIndex]->_double;
                  case STRING_VAL:
                    return std::strcmp((*a->cells)[fieldIndex]->_string,
                                       (*b->cells)[fieldIndex]->_string) > 0;
                  default:throw IllegalStateException("Internal error.");
                }
              });
  }

  this->first();
}

void DataSets::MemoryDataSet::filter(const FilterOptions &options) {
  if (options.options.empty()) {
    dataValidityChanged = false;

    for (auto &iter : this->data) {
      iter->valid = true;
    }

    this->first();

    return;
  }

  dataValidityChanged = true;

  size_t i = 0;
  std::string toCompare;
  for (auto iter : this->data) {
    bool valid = true;

    size_t optionCounter = 0;
    for (auto filter : options.options) {
      if (!valid) {
        break;
      }

      switch (this->fields[filter.fieldIndex]->getFieldType()) {
        case INTEGER_VAL:
          toCompare = std::to_string(
              (*iter->cells)[filter.fieldIndex]->_integer);
          break;
        case DOUBLE_VAL:
          toCompare = std::to_string(
              (*iter->cells)[filter.fieldIndex]->_double);
          break;
        case STRING_VAL:
          toCompare = (*iter->cells)[filter.fieldIndex]->_string;
          break;
        default:throw IllegalStateException("Internal error.");
      }

      for (const auto &searchString : filter.searchString) {
          switch (filter.filterOption) {
            case EQUALS:valid = toCompare == searchString;
              break;
            case STARTS_WITH:
              valid = std::strncmp(toCompare.c_str(),
                                   searchString.c_str(),
                                   searchString.size()) == 0;
              break;
            case CONTAINS:
              valid = toCompare.find(searchString)
                  != std::string::npos;
              break;
            case ENDS_WITH:valid = Utilities::endsWith(toCompare, searchString);
              break;
            case NOT_CONTAINS:
              valid = toCompare.find(searchString)
                  == std::string::npos;
              break;
            case NOT_STARTS_WITH:
              valid = std::strncmp(toCompare.c_str(),
                                   searchString.c_str(),
                                   searchString.size()) != 0;
              break;
            case NOT_ENDS_WITH:
              valid = !Utilities::endsWith(toCompare,
                                           searchString);
              break;
          }
          if (valid) {
              break;
          }
      }

      optionCounter++;
    }

    this->data[i]->valid = valid;
    i++;
  }

  this->first();
}

DataSets::BaseField *DataSets::MemoryDataSet::fieldByName(
    const std::string &name) {
  for (auto &field : fields) {
    if (field->getFieldName() == name) {
      return field;
    }
  }

  throw InvalidArgumentException(("There's no field named: " + name).c_str());
}

DataSets::BaseField *DataSets::MemoryDataSet::fieldByIndex(uint64_t index) {
  return fields.at(index);
}

bool DataSets::MemoryDataSet::eof() {
  return this->currentRecord >= this->data.size();
}

DataSets::MemoryDataSet::~MemoryDataSet() {
  this->close();
}

void DataSets::MemoryDataSet::setFieldTypes(std::vector<ValueType> types) {
  createFields(this->dataProvider->getColumnNames(), types);
}

void DataSets::MemoryDataSet::setData(void *data, uint64_t index, ValueType type) {
  switch (type) {
    case INTEGER_VAL:
      (*this->data[currentRecord]->cells)[index]->_integer
          = *reinterpret_cast<int *>(data);
      break;
    case DOUBLE_VAL:
      (*this->data[currentRecord]->cells)[index]->_double
          = *reinterpret_cast<int *>(data);
      break;
    case STRING_VAL:
      delete[] (*this->data[currentRecord]->cells)[index]->_string;
      (*this->data[currentRecord]->cells)[index]->_string
          = reinterpret_cast<char *>(data);
      break;
    default:throw IllegalStateException("Invalid value type.");
  }

}

void DataSets::MemoryDataSet::append() {
  auto newRecord = new DataSetRowCells();
  for (size_t i = 0; i < this->getFieldNames().size(); ++i) {
    auto dataContainer = new DataContainer();
    switch (fields[i]->getFieldType()) {
      case INTEGER_VAL:dataContainer->_integer = 0;
        break;
      case DOUBLE_VAL:dataContainer->_double = 0;
        break;
      case STRING_VAL:dataContainer->_string = nullptr;
        break;
      default:throw IllegalStateException("Internal error.");
    }
    newRecord->push_back(dataContainer);
  }

  this->data.push_back(new DataSetRow {true, newRecord});
  this->last();
}

void DataSets::MemoryDataSet::appendDataProvider(
    DataProviders::BaseDataProvider *provider) {
  if (!isOpen) {
    throw IllegalStateException("Dataset is not open.");
  }

  if (provider == nullptr) {
    throw InvalidArgumentException("Data provider has not been set.");
  }

  this->dataProvider = provider;

  this->loadData();
}








