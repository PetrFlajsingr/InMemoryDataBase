//
// Created by Petr Flajsingr on 27/08/2018.
//
#include "DoubleField.h"
#include "MemoryDataSet.h"

DataSets::DoubleField::DoubleField(std::string_view fieldName,
                                   gsl::index index,
                                   BaseDataSet *dataSet)
    : BaseField(fieldName,
                index, dataSet) {}

ValueType DataSets::DoubleField::getFieldType() const {
  return ValueType::Double;
}

void DataSets::DoubleField::setAsString(std::string_view value) {
  data = Utilities::stringToDouble(value);
  setData(&data, getFieldType());
}

std::string DataSets::DoubleField::getAsString() const {
  return std::to_string(data);
}

void DataSets::DoubleField::setValue(void *data) {
  this->data = *reinterpret_cast<double *>(data);
}

void DataSets::DoubleField::setAsDouble(double value) {
  data = value;
  setData(&data, getFieldType());
}

double DataSets::DoubleField::getAsDouble() const {
  return data;
}

std::function<int8_t(const DataSetRow *,
                     const DataSetRow *)> DataSets::DoubleField::getCompareFunction() const {
  return [this](const DataSetRow *a,
                const DataSetRow *b) {
    return Utilities::compareDouble((*a)[getIndex() & 0x00FFFF]._double,
                                    (*b)[getIndex() & 0x00FFFF]._double);
  };
}


