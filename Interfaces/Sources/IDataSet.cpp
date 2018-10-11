//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "IDataSet.h"
#include "IField.h"

void IDataSet::setFieldData(IField *field, void *data)  {
    field->setValue(data);
}

IDataSet::~IDataSet() {
    for (auto tmp : this->fields) {
        delete tmp;
    }
}
