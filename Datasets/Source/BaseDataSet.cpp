//
// Created by Petr Flajsingr on 27/08/2018.
//

#include <BaseDataSet.h>

#include "BaseDataSet.h"
#include "BaseField.h"

void DataSets::BaseDataSet::setFieldData(BaseField *field, void *data) {
  field->setValue(data);
}

DataSets::BaseDataSet::~BaseDataSet() {
  for (auto tmp : fields) {
    delete tmp;
  }
}
void DataSets::BaseDataSet::findFirst(std::string fieldName, const std::string &searchString) {
  findFirst(fieldByName(fieldName)->getIndex(), searchString);
}
