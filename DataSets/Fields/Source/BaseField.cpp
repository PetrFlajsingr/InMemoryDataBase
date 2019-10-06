//
// Created by Petr Flajsingr on 2019-01-30.
//

#include <BaseDataSet.h>
#include <BaseField.h>

void DataSets::BaseField::setData(void *data, ValueType type) { dataSet->setData(data, index, type); }
