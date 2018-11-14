//
// Created by Petr Flajsingr on 08/11/2018.
//

#include <CurrencyField.h>

#include "MemoryDataSet.h"

void DataSets::CurrencyField::setValue(void *data) {
  this->data = *reinterpret_cast<Currency *>(data);
}

ValueType DataSets::CurrencyField::getFieldType() {
  return CURRENCY;
}

void DataSets::CurrencyField::setAsString(const std::string &value) {
  data = dec::fromString<Currency>(value);
  setData(&data, getFieldType());
}

std::string DataSets::CurrencyField::getAsString() {
  return dec::toString(data);
}

std::function<int8_t (DataSets::DataSetRow *, DataSets::DataSetRow *)> DataSets::CurrencyField::getCompareFunction() {
  return [this](const DataSetRow *a,
                const DataSetRow *b) {
    if ((*a->cells)[index]->_currency
        == (*b->cells)[index]->_currency) {
      return 0;
    }
    if ((*a->cells)[index]->_currency
        < (*b->cells)[index]->_currency) {
      return -1;
    }
    return 1;
  };

}
void DataSets::CurrencyField::setAsCurrency(Currency &value) {
  data = value;
  BaseField::setData(&data, getFieldType());
}
Currency DataSets::CurrencyField::getAsCurrency() {
  return data;
}
DataSets::CurrencyField::CurrencyField(const std::string &fieldName, DataSets::BaseDataSet *dataset, uint64_t index)
    : BaseField(fieldName, dataset, index) {}
