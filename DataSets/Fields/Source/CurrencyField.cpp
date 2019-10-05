//
// Created by Petr Flajsingr on 08/11/2018.
//

#include <CurrencyField.h>
#include "MemoryDataSet.h"

DataSets::CurrencyField::CurrencyField(std::string_view fieldName, gsl::index index, BaseDataSet *dataSet)
    : BaseField(fieldName, index, dataSet) {}

void DataSets::CurrencyField::setValue(const void *data) {
    this->value = *reinterpret_cast<const Currency *>(data);
}

ValueType DataSets::CurrencyField::getFieldType() const {
  return ValueType::Currency;
}

void DataSets::CurrencyField::setAsString(std::string_view newValue) {
    value = dec::fromString<Currency>(std::string(newValue));
    setData(&value, getFieldType());
}

std::string DataSets::CurrencyField::getAsString() const {
    return dec::toString(value);
}

std::function<int8_t(const DataSetRow *,
                     const DataSetRow *)> DataSets::CurrencyField::getCompareFunction() const {
  return [this](const DataSetRow *a, const DataSetRow *b) {
    return Utilities::compareCurrency(*(*a)[getIndex() & 0x00FFFF]._currency,
                                      *(*b)[getIndex() & 0x00FFFF]._currency);
  };
}

void DataSets::CurrencyField::setAsCurrency(const Currency &newValue) {
    value = newValue;
    BaseField::setData(&value, getFieldType());
}

Currency DataSets::CurrencyField::getAsCurrency() const {
    return value;
}
