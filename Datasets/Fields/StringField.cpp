//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "StringField.h"

StringField::StringField(const std::string &fieldName, IDataset *dataset, unsigned long index) : IField(fieldName,
                                                                                                        dataset,
                                                                                                        index) {}

ValueType StringField::getFieldType() {
    return STRING_VAL;
}

void StringField::setAsString(const std::string& value) {
    this->data = value;
    IField::setDatasetData((u_int8_t *) this->data.c_str(), getFieldType());
}

std::string StringField::getAsString() {
    return this->data;
}

void StringField::setValue(u_int8_t *data) {
    auto pointer = reinterpret_cast<char *>(data);
    this->data = std::string(pointer);
}