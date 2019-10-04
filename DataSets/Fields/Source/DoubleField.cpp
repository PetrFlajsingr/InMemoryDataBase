//
// Created by Petr Flajsingr on 27/08/2018.
//
#include "DoubleField.h"
#include "MemoryDataSet.h"

DataSets::DoubleField::DoubleField(std::string_view fieldName, gsl::index index, BaseDataSet *dataSet)
    : BaseField(fieldName, index, dataSet) {}

ValueType DataSets::DoubleField::getFieldType() const {
  return ValueType::Double;
}

void DataSets::DoubleField::setAsString(std::string_view newValue) {
    value = Utilities::stringToDouble(newValue);
    setData(&value, getFieldType());
}

std::string DataSets::DoubleField::getAsString() const {
    return std::to_string(value);
}

void DataSets::DoubleField::setValue(const void *data) {
    this->value = *reinterpret_cast<const double *>(data);
}

void DataSets::DoubleField::setAsDouble(double newValue) {
    value = newValue;
    setData(&value, getFieldType());
}

double DataSets::DoubleField::getAsDouble() const {
    return value;
}

std::function<int8_t(const DataSetRow *,
                     const DataSetRow *)> DataSets::DoubleField::getCompareFunction() const {
  return [this](const DataSetRow *a, const DataSetRow *b) {
    return Utilities::compareDouble((*a)[getIndex() & 0x00FFFF]._double,
                                    (*b)[getIndex() & 0x00FFFF]._double);
  };
}


