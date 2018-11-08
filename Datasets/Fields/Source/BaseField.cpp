//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "Types.h"
#include "BaseField.h"

void DataSets::BaseField::setData(void *data, ValueType type) {
  dataSet->setData(data, index, type);
}
