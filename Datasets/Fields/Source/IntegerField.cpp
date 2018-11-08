//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "IntegerField.h"
#include "MemoryDataSet.h"

DataSets::IntegerField::IntegerField(const std::string &fieldName,
                                     BaseDataSet *dataset,
                                     uint64_t index) : BaseField(fieldName,
                                                                 dataset,
                                                                 index) {}

ValueType DataSets::IntegerField::getFieldType() {
  return INTEGER_VAL;
}

void DataSets::IntegerField::setAsString(const std::string &value) {
  data = Utilities::stringToInt(value);
  BaseField::setData(&data, getFieldType());
}

std::string DataSets::IntegerField::getAsString() {
  return std::to_string(data);
}

void DataSets::IntegerField::setValue(void *data) {
  this->data = *reinterpret_cast<int *>(data);
}

void DataSets::IntegerField::setAsInteger(int value) {
  data = value;
  BaseField::setData(&data, getFieldType());
}

int DataSets::IntegerField::getAsInteger() {
  return data;
}

std::function<bool(DataSets::DataSetRow *,
                   DataSets::DataSetRow *)> DataSets::IntegerField::getCompareFunction(SortOrder order) {
  if (order == SortOrder::ASCENDING) {
    return [this, order](const DataSetRow *a,
                         const DataSetRow *b) {
      return (*a->cells)[index]->_integer
          < (*b->cells)[index]->_integer;
    };
  } else {
    return [this, order](const DataSetRow *a,
                         const DataSetRow *b) {
      return (*a->cells)[index]->_integer
          > (*b->cells)[index]->_integer;
    };
  }
}
