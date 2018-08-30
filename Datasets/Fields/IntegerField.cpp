//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "IntegerField.h"
#include "../../Misc/Utilities.h"

IntegerField::IntegerField(const std::string &fieldName, IDataset *dataset, unsigned long index) : IField(fieldName,
                                                                                                          dataset,
                                                                                                          index) {}

ValueType IntegerField::getFieldType() {
    return INTEGER_VAL;
}

void IntegerField::setAsString(const std::string &value) {
    this->data = Utilities::stringToInt(value);
    IField::setDatasetData((u_int8_t *) &this->data, getFieldType());
}

std::string IntegerField::getAsString() {
    return std::to_string(this->data);
}

void IntegerField::setValue(void *data) {
    this->data = *(int*)data;
}

void IntegerField::setAsInteger(int value) {
    this->data = value;
    IField::setDatasetData((u_int8_t *) &this->data, getFieldType());
}

int IntegerField::getAsInteger() {
    return this->data;
}