//
// Created by Petr Flajsingr on 27/08/2018.
//

#include <BaseDataSet.h>

void DataSets::BaseDataSet::setFieldData(BaseField *field, const void *data) { field->setValue(data); }

gsl::index DataSets::BaseDataSet::getColumnCount() const { return fields.size(); }

std::pair<gsl::index, gsl::index> DataSets::BaseField::convertIndex(const BaseField &field) {
  return std::make_pair((field.getIndex() & maskTableIndex) >> maskTableShift, field.getIndex() & maskColumnIndex);
}