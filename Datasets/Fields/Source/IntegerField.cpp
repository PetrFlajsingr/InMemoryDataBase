//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "IntegerField.h"

DataSets::IntegerField::IntegerField(const std::string &fieldName,
                                     BaseDataSet *dataset,
                                     uint64_t index) : BaseField(fieldName,
                                                              dataset,
                                                              index) {}

ValueType DataSets::IntegerField::getFieldType() {
  return INTEGER_VAL;
}

void DataSets::IntegerField::setAsString(const std::string &value) {
  this->data = Utilities::stringToInt(value);
  BaseField::setDataSetData(&this->data, getFieldType());
}

std::string DataSets::IntegerField::getAsString() {
  return std::to_string(this->data);
}

void DataSets::IntegerField::setValue(void *data) {
  this->data = *reinterpret_cast<int *>(data);
}

void DataSets::IntegerField::setAsInteger(int value) {
  this->data = value;
  BaseField::setDataSetData(&this->data, getFieldType());
}

int DataSets::IntegerField::getAsInteger() {
  return this->data;
}
