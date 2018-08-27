//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "IDataset.h"
#include "IField.h"

void IDataset::setFieldData(IField *field, u_int8_t *data)  {
    field->setValue(data);
}