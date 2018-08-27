//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "IDataset.h"
#include "IField.h"

void IDataset::setFieldData(IField *field, void *data)  {
    field->setValue(data);
}