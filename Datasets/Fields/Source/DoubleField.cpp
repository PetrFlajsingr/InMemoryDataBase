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

ValueType DataSets::DoubleField::getFieldType() {
  return DOUBLE_VAL;
}

void DataSets::DoubleField::setAsString(const std::string &value) {
  data = Utilities::stringToDouble(value);
  setData(&data, getFieldType());
}

std::string DataSets::DoubleField::getAsString() {
  return std::to_string(data);
}

void DataSets::DoubleField::setValue(void *data) {
  this->data = *reinterpret_cast<double *>(data);
}

void DataSets::DoubleField::setAsDouble(double value) {
  data = value;
  setData(&data, getFieldType());
}

double DataSets::DoubleField::getAsDouble() {
  return data;
}

std::function<bool(DataSets::DataSetRow *,
                   DataSets::DataSetRow *)> DataSets::DoubleField::getCompareFunction(SortOrder order) {
  if (order == SortOrder::ASCENDING) {
    return [this, order](const DataSetRow *a,
                         const DataSetRow *b) {
      return (*a->cells)[index]->_double
          < (*b->cells)[index]->_double;
    };
  } else {
    return [this, order](const DataSetRow *a,
                         const DataSetRow *b) {
      return (*a->cells)[index]->_double
          > (*b->cells)[index]->_double;
    };
  }
}


