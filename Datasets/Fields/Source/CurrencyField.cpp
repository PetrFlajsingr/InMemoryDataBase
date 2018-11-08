//
// Created by Petr Flajsingr on 08/11/2018.
//

#include "CurrencyField.h"

void DataSets::CurrencyField::setValue(void *data) {
  throw NotImplementedException();
}

ValueType DataSets::CurrencyField::getFieldType() {
  return CURRENCY;
}

void DataSets::CurrencyField::setAsString(const std::string &value) {
  throw NotImplementedException();
}

std::string DataSets::CurrencyField::getAsString() {
  throw NotImplementedException();
}

std::function<bool(DataSets::DataSetRow *, DataSets::DataSetRow *)> DataSets::CurrencyField::getCompareFunction(
    SortOrder order) {
  throw NotImplementedException();
}
