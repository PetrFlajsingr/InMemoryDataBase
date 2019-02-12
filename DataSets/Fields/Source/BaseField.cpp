//
// Created by Petr Flajsingr on 2019-01-30.
//

#include <BaseField.h>
#include <BaseDataSet.h>
#include <Utilities.h>
#include <MemoryDataSet.h>

void DataSets::BaseField::setData(void *data, ValueType type) {
  dataSet->setData(data, index, type);
}