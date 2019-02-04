//
// Created by Petr Flajsingr on 27/08/2018.
//

#include <BaseDataSet.h>

#include "BaseDataSet.h"
#include "BaseField.h"

void DataSets::BaseDataSet::setFieldData(BaseField *field, void *data) {
  field->setValue(data);
}

gsl::index DataSets::BaseDataSet::getColumnCount() const {
  return columnCount;
}
