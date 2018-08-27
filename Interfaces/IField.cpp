//
// Created by Petr Flajsingr on 27/08/2018.
//

#include <stdint.h>
#include "../Misc/Types.h"
#include "IField.h"
#include "IDataset.h"

void IField::setDatasetData(u_int8_t * data, ValueType type) {
    this->dataset->setData(data, this->index, type);
}