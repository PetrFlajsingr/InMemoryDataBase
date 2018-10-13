//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "StringField.h"

DataSets::StringField::StringField(const std::string &fieldName,
                                   BaseDataSet *dataset,
                                   uint64_t index) : BaseField(fieldName,
                                                            dataset,
                                                            index) {}

ValueType DataSets::StringField::getFieldType() {
  return STRING_VAL;
}

void DataSets::StringField::setAsString(const std::string &value) {
  this->data = value;
  BaseField::setDataSetData(Utilities::copyStringToNewChar(value),
                         getFieldType());
}

std::string DataSets::StringField::getAsString() {
  return this->data;
}

void DataSets::StringField::setValue(void *data) {
  if (data == nullptr) {
    this->data = "";
    return;
  }
  this->data = std::string(reinterpret_cast<char *>(data));
}
