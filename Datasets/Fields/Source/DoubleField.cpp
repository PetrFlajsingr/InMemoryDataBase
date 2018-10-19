//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "DoubleField.h"

DataSets::DoubleField::DoubleField(const std::string &fieldName,
                                   BaseDataSet *dataset,
                                   uint64_t index) : BaseField(fieldName,
                                                            dataset,
                                                            index) {}

ValueType DataSets::DoubleField::getFieldType() {
  return DOUBLE_VAL;
}

void DataSets::DoubleField::setAsString(const std::string &value) {
  data = Utilities::stringToDouble(value);
  setDataSetData(&data, getFieldType());
}

std::string DataSets::DoubleField::getAsString() {
  return std::to_string(data);
}

void DataSets::DoubleField::setValue(void *data) {
  this->data = *reinterpret_cast<double *>(data);
}

void DataSets::DoubleField::setAsDouble(double value) {
  data = value;
  setDataSetData(&data, getFieldType());
}

double DataSets::DoubleField::getAsDouble() {
  return data;
}
