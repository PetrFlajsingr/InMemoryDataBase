//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "StringField.h"

DataSets::StringField::StringField(const std::string &fieldName,
                                   IDataSet *dataset,
                                   uint64_t index) : IField(fieldName,
                                                            dataset,
                                                            index) {}

ValueType DataSets::StringField::getFieldType() {
  return STRING_VAL;
}

void DataSets::StringField::setAsString(const std::string &value) {
  this->data = value;
  IField::setDataSetData(Utilities::copyStringToNewChar(value),
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
