//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "Types.h"
#include "IField.h"

void DataSets::IField::setDataSetData(void *data, ValueType type) {
  this->dataset->setData(data, this->index, type);
}
