//
// Created by Petr Flajsingr on 2019-01-27.
//

#include <DateTimeField.h>

DataSets::DateTimeField::DateTimeField(std::string_view fieldName, gsl::index index, BaseDataSet *dataSet)
    : BaseField(fieldName, index, dataSet) {}

ValueType DataSets::DateTimeField::getFieldType() const {
  return ValueType::DateTime;
}

void DataSets::DateTimeField::setAsString(std::string_view newValue) {
    value.fromString(newValue);
    setData(&value, ValueType::DateTime);
}

std::string DataSets::DateTimeField::getAsString() const {
    return value.toString();
}

std::function<int8_t(const DataSetRow *,
                     const DataSetRow *)> DataSets::DateTimeField::getCompareFunction() const {
  return [this](const DataSetRow *a, const DataSetRow *b) {
    return Utilities::compareDateTime(*(*a)[getIndex() & 0x00FFFF]._dateTime,
                                      *(*b)[getIndex() & 0x00FFFF]._dateTime);
  };
}

void DataSets::DateTimeField::setValue(const void *data) {
    this->value = *reinterpret_cast<const DateTime *>(data);
}

void DataSets::DateTimeField::setAsDateTime(const DateTime &newValue) {
    value = newValue;
}

DateTime DataSets::DateTimeField::getAsDateTime() const {
    return value;
}
