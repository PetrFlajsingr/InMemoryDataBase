//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "IntegerField.h"
#include "MemoryDataSet.h"

DataSets::IntegerField::IntegerField(std::string_view fieldName, gsl::index index, BaseDataSet *dataSet)
    : BaseField(fieldName, index, dataSet) {}

ValueType DataSets::IntegerField::getFieldType() const { return ValueType::Integer; }

void DataSets::IntegerField::setAsString(std::string_view newValue) {
  value = Utilities::stringToInt(newValue);
  BaseField::setData(&value, getFieldType());
}

std::string DataSets::IntegerField::getAsString() const { return std::to_string(value); }

void DataSets::IntegerField::setValue(const void *data) { this->value = *reinterpret_cast<const int *>(data); }

void DataSets::IntegerField::setAsInteger(int newValue) {
  value = newValue;
  BaseField::setData(&value, getFieldType());
}

int DataSets::IntegerField::getAsInteger() const { return value; }

std::function<int8_t(const DataSetRow *, const DataSetRow *)> DataSets::IntegerField::getCompareFunction() const {
  return [this](const DataSetRow *a, const DataSetRow *b) {
    return Utilities::compareInt((*a)[getIndex() & 0x00FFFF]._integer, (*b)[getIndex() & 0x00FFFF]._integer);
  };
}
