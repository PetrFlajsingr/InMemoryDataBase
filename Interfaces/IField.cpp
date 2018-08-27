//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "../Misc/Types.h"
#include "IField.h"
#include "IDataset.h"

void IField::setDatasetData(void * data, ValueType type) {
    this->dataset->setData(data, this->index, type);
}