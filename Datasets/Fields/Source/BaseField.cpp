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
