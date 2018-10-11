//
// Created by Petr Flajsingr on 27/08/2018.
//

#include "StringField.h"
#include "Utilities.h"

StringField::StringField(const std::string &fieldName,
        IDataSet *dataset,
        uint64_t index) : IField(fieldName,
                dataset,
                index) {}

ValueType StringField::getFieldType() {
    return STRING_VAL;
}

void StringField::setAsString(const std::string& value) {
    this->data = value;
    IField::setDatasetData(Utilities::copyStringToNewChar(value),
            getFieldType());
}

std::string StringField::getAsString() {
    return this->data;
}

void StringField::setValue(void *data) {
    if (data == nullptr) {
        this->data = "";
        return;
    }
    this->data = std::string(reinterpret_cast<char *>(data));
}
