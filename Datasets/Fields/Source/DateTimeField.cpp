//
// Created by Petr Flajsingr on 2019-01-27.
//

#include <DateTimeField.h>

ValueType DataSets::DateTimeField::getFieldType() const {
  return DateTimeValue;
}

void DataSets::DateTimeField::setAsString(const std::string &value) {
  data.fromString(value);
}

std::string DataSets::DateTimeField::getAsString() const {
  return data.toString();
}

std::function<int8_t(DataSets::DataSetRow *, DataSets::DataSetRow *)> DataSets::DateTimeField::getCompareFunction() {
  return [this](const DataSetRow *a,
                const DataSetRow *b) {
    if (*(*a->cells)[index]->_dateTime
        == *(*b->cells)[index]->_dateTime) {
      return 0;
    }
    if (*(*a->cells)[index]->_dateTime
        < *(*b->cells)[index]->_dateTime) {
      return -1;
    }
    return 1;
  };
}

void DataSets::DateTimeField::setValue(void *data) {
  this->data = *reinterpret_cast<DateTime *>(data);
}

void DataSets::DateTimeField::setAsDateTime(const DateTime &dateTime) {
  data = dateTime;
}

DateTime DataSets::DateTimeField::getAsDateTime() const {
  return data;
}

DataSets::DateTimeField::DateTimeField(const std::string &fieldName, DataSets::BaseDataSet *dataset, uint64_t index)
    : BaseField(fieldName, dataset, index), data(DateTime(DateTime_s)) {}
