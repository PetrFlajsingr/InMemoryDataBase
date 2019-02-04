//
// Created by Petr Flajsingr on 2019-01-27.
//

#include <DateTimeField.h>

DataSets::DateTimeField::DateTimeField(std::string_view fieldName,
                                       gsl::index index,
                                       BaseDataSet *dataSet)
    : BaseField(fieldName, index, dataSet) {}

ValueType DataSets::DateTimeField::getFieldType() const {
  return ValueType::DateTime;
}

void DataSets::DateTimeField::setAsString(std::string_view value) {
  data.fromString(value);
}

std::string_view DataSets::DateTimeField::getAsString() const {
  return data.toString();
}

std::function<int8_t(const DataSets::DataSetRow &,
                     const DataSets::DataSetRow &)> DataSets::DateTimeField::getCompareFunction() {
  return [this](const DataSetRow &a,
                const DataSetRow &b) {
    return Utilities::compareDateTime(*a.cells[index]._dateTime,
                                      *b.cells[index]._dateTime);
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
