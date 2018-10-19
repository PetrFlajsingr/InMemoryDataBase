//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "Types.h"
#include "BaseField.h"

void DataSets::BaseField::setDataSetData(void *data, ValueType type) {
  this->dataSet->setData(data, this->index, type);
}
