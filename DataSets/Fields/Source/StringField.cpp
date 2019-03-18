//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "StringField.h"
#include "MemoryDataSet.h"

DataSets::StringField::StringField(std::string_view fieldName,
                                   gsl::index index,
                                   BaseDataSet *dataSet)
    : BaseField(fieldName,
                index, dataSet) {}

ValueType DataSets::StringField::getFieldType() const {
  return ValueType::String;
}

void DataSets::StringField::setAsString(std::string_view value) {
  data = value;
  BaseField::setData(Utilities::copyStringToNewChar(value),
                     getFieldType());
}

std::string DataSets::StringField::getAsString() const {
  return data;
}

void DataSets::StringField::setValue(void *data) {
  if (data == nullptr) {
    this->data = "";
    return;
  }
  this->data = std::string(reinterpret_cast<gsl::zstring<>>(data));
}

std::function<int8_t(const DataSetRow *,
                     const DataSetRow *)> DataSets::StringField::getCompareFunction() const {
  return [this](const DataSetRow *a,
                const DataSetRow *b) {
    return Utilities::compareString((*a)[getIndex() & 0x00FFFF]._string,
                                    (*b)[getIndex() & 0x00FFFF]._string);
  };
}