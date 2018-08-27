//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "DoubleField.h"
#include "../../Misc/Utilities.h"

DoubleField::DoubleField(const std::string &fieldName, IDataset *dataset, unsigned long index) : IField(fieldName,
                                                                                                        dataset,
                                                                                                        index) {}

ValueType DoubleField::getFieldType() {
    return DOUBLE_VAL;
}

void DoubleField::setAsString(const std::string &value) {
    this->data = Utilities::StringToDouble(value);
    IField::setDatasetData((u_int8_t *) &this->data, getFieldType());
}

std::string DoubleField::getAsString() {
    return std::to_string(this->data);
}

void DoubleField::setValue(u_int8_t *data) {
    std::memcpy(&this->data, data, sizeof(double));
}

void DoubleField::setAsDouble(double value) {
    this->data = value;
    IField::setDatasetData((u_int8_t *) &this->data, getFieldType());
}

double DoubleField::getAsDouble() {
    return this->data;
}