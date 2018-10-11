//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "DoubleField.h"
#include "Utilities.h"

DoubleField::DoubleField(const std::string &fieldName,
        IDataSet *dataset,
        uint64_t index) : IField(fieldName,
                dataset,
                index) {}

ValueType DoubleField::getFieldType() {
    return DOUBLE_VAL;
}

void DoubleField::setAsString(const std::string &value) {
    this->data = Utilities::stringToDouble(value);
    IField::setDatasetData(&this->data, getFieldType());
}

std::string DoubleField::getAsString() {
    return std::to_string(this->data);
}

void DoubleField::setValue(void *data) {
    this->data = *reinterpret_cast<double *>(data);
}

void DoubleField::setAsDouble(double value) {
    this->data = value;
    IField::setDatasetData(&this->data, getFieldType());
}

double DoubleField::getAsDouble() {
    return this->data;
}
