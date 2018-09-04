//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "StringField.h"
#include "Utilities.h"

StringField::StringField(const std::string &fieldName, IDataset *dataset, unsigned long index) : IField(fieldName,
                                                                                                        dataset,
                                                                                                        index) {}

ValueType StringField::getFieldType() {
    return STRING_VAL;
}

void StringField::setAsString(const std::string& value) {
    this->data = value;
    IField::setDatasetData((u_int8_t *) Utilities::copyStringToNewChar(value), getFieldType());
}

std::string StringField::getAsString() {
    return this->data;
}

void StringField::setValue(void *data) {
    if(data == nullptr) {
        this->data = "";
        return;
    }
    this->data = std::string((char*)data);
}