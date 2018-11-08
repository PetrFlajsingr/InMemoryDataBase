//
// Created by Petr Flajsingr on 08/11/2018.
//

#include "MemoryDataSet.h"
#include "CurrencyField.h"

void DataSets::CurrencyField::setValue(void *data) {
  throw NotImplementedException();
}

ValueType DataSets::CurrencyField::getFieldType() {
  return CURRENCY;
}

void DataSets::CurrencyField::setAsString(const std::string &value) {
  data = dec::fromString<Currency>(value);
  setDataSetData(&data, getFieldType());
}

std::string DataSets::CurrencyField::getAsString() {
  return dec::toString(data);
}

std::function<bool(DataSets::DataSetRow *, DataSets::DataSetRow *)> DataSets::CurrencyField::getCompareFunction(
    SortOrder order) {
  return [this, order](const DataSetRow *a,
                       const DataSetRow *b) {
    if (order == SortOrder::ASCENDING) {
      return (*a->cells)[index]->_currency
          < (*b->cells)[index]->_currency;
    } else {
      return (*a->cells)[index]->_currency
          > (*b->cells)[index]->_currency;
    }
  };
}
void DataSets::CurrencyField::setAsCurrency(Currency &value) {
  data = value;
}
Currency DataSets::CurrencyField::getAsCurrency() {
  return data;
}
