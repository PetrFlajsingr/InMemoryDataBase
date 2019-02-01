//
// Created by Petr Flajsingr on 27/08/2018.
//
#include "DoubleField.h"
#include "MemoryDataSet.h"

DataSets::DoubleField::DoubleField(const std::string &fieldName,
                                   BaseDataSet *dataset,
                                   uint64_t index) : BaseField(fieldName,
                                                               dataset,
                                                               index) {}

ValueType DataSets::DoubleField::getFieldType() const {
  return ValueType::Double;
}

void DataSets::DoubleField::setAsString(const std::string &value) {
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

std::function<int8_t(DataSets::DataSetRow *,
                     DataSets::DataSetRow *)> DataSets::DoubleField::getCompareFunction() {
  return [this](const DataSetRow *a,
                const DataSetRow *b) {
    if ((*a->cells)[index]->_double
        == (*b->cells)[index]->_double) {
      return 0;
    }
    if ((*a->cells)[index]->_double
        < (*b->cells)[index]->_double) {
      return -1;
    }
    return 1;
  };
}


