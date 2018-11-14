//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "StringField.h"
#include "MemoryDataSet.h"

DataSets::StringField::StringField(const std::string &fieldName,
                                   BaseDataSet *dataset,
                                   uint64_t index) : BaseField(fieldName,
                                                               dataset,
                                                               index) {}

ValueType DataSets::StringField::getFieldType() {
  return STRING_VAL;
}

void DataSets::StringField::setAsString(const std::string &value) {
  data = value;
  BaseField::setData(Utilities::copyStringToNewChar(value),
                     getFieldType());
}

std::string DataSets::StringField::getAsString() {
  return data;
}

void DataSets::StringField::setValue(void *data) {
  if (data == nullptr) {
    this->data = "";
    return;
  }
  this->data = std::string(reinterpret_cast<char *>(data));
}

std::function<int8_t (DataSets::DataSetRow *,
                   DataSets::DataSetRow *)> DataSets::StringField::getCompareFunction() {
    return [this](const DataSetRow *a,
                         const DataSetRow *b) {
      return strcmp((*a->cells)[index]->_string,
                    (*b->cells)[index]->_string) < 0;
    };
}
