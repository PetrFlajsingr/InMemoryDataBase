//
// Created by Petr Flajsingr on 27/08/2018.
//

#include <IntegerField.h>
#include <DoubleField.h>
#include <CurrencyField.h>
#include <StringField.h>
#include "Types.h"
#include "BaseField.h"

void DataSets::BaseField::setData(void *data, ValueType type) {
  dataSet->setData(data, index, type);
}

template <typename T>
T DataSets::BaseField::getValue() {
  std::string errorMsg = std::string("Unsupported type: ") + typeid(T).name();
  throw UnsupportedOperationException(errorMsg.c_str());
}

template <>
int DataSets::BaseField::getValue() {
  return dynamic_cast<DataSets::IntegerField*>(this)->getAsInteger();
}

template <>
double DataSets::BaseField::getValue() {
  return dynamic_cast<DataSets::DoubleField*>(this)->getAsDouble();
}

template <>
Currency DataSets::BaseField::getValue() {
  return dynamic_cast<DataSets::CurrencyField*>(this)->getAsCurrency();
}
