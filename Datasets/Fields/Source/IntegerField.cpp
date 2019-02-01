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

ValueType DataSets::IntegerField::getFieldType() const {
  return ValueType::Integer;
}

void DataSets::IntegerField::setAsString(const std::string &value) {
  data = Utilities::stringToInt(value);
  BaseField::setData(&data, getFieldType());
}

std::string DataSets::IntegerField::getAsString() const {
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

std::function<int8_t(DataSets::DataSetRow *,
                     DataSets::DataSetRow *)> DataSets::IntegerField::getCompareFunction() {
  return [this](const DataSetRow *a,
                const DataSetRow *b) {
    if ((*a->cells)[index]->_integer
        == (*b->cells)[index]->_integer) {
      return 0;
    }
    if ((*a->cells)[index]->_integer
        < (*b->cells)[index]->_integer) {
      return -1;
    }
    return 1;
  };
}
