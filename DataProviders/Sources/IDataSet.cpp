//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "IDataSet.h"
#include "IField.h"

void DataSets::IDataSet::setFieldData(IField *field, void *data) {
  field->setValue(data);
}

DataSets::IDataSet::~IDataSet() {
  for (auto tmp : this->fields) {
    delete tmp;
  }
}
