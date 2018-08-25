//
// Created by Petr Flajsingr on 25/08/2018.
//

#include "Field.h"

Field::Field(const std::string name) : fieldName(name) {}

Field::~Field() = default;

void Field::setValue(std::string value) {
    if(Utilities::isInteger(value)) {
        this->integerData = Utilities::StringToInt(value);
        this->fieldType = INTEGER_VAL;
    } else if(Utilities::isDouble(value)) {
        this->doubleData = Utilities::StringToDouble(value);
        this->fieldType = DOUBLE_VAL;
    } else {
        this->stringData = value;
        this->fieldType = STRING_VAL;
    }
}




