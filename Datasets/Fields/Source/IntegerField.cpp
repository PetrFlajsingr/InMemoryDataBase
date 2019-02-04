//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "IntegerField.h"
#include "MemoryDataSet.h"

DataSets::IntegerField::IntegerField(std::string_view fieldName,
                                     gsl::index index,
                                     BaseDataSet *dataSet)
    : BaseField(fieldName,
                index, dataSet) {}

ValueType DataSets::IntegerField::getFieldType() const {
  return ValueType::Integer;
}

void DataSets::IntegerField::setAsString(std::string_view value) {
  data = Utilities::stringToInt(value);
  BaseField::setData(&data, getFieldType());
}

std::string_view DataSets::IntegerField::getAsString() const {
  return std::to_string(data);
}

void DataSets::IntegerField::setValue(void *data) {
  this->data = *reinterpret_cast<int *>(data);
}

void DataSets::IntegerField::setAsInteger(int value) {
  data = value;
  BaseField::setData(&data, getFieldType());
}

int DataSets::IntegerField::getAsInteger() const {
  return data;
}

std::function<int8_t(const DataSets::DataSetRow &,
                     const DataSets::DataSetRow &)> DataSets::IntegerField::getCompareFunction() {
  return [this](const DataSetRow &a,
                const DataSetRow &b) {
    return Utilities::compareInt(a.cells[index]._integer,
                                 b.cells[index]._integer);
  };
}
