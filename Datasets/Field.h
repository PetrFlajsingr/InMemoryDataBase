//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef CSV_READER_FIELD_H
#define CSV_READER_FIELD_H


#include <string>
#include <utility>
#include "../Misc/Utilities.h"
#include "../Misc/Exceptions.h"

/**
 * Trida uchovavajici zaznamy datasetu pro iteraci mezi nimi.
 * Predpoklada se prubezne nastaveni pri jakekoliv zmene zaznamu.
 */
class Field {
    enum ValueType {NONE, INTEGER_VAL, DOUBLE_VAL, STRING_VAL};
private:
    ValueType fieldType = NONE;

    std::string stringData;

    int integerData;

    double doubleData;
public:
    Field();

    ~Field();

    void setValue(std::string value);

    void setAsString(std::string value) {
        this->stringData = std::move(value);

        this->fieldType = STRING_VAL;
    }

    std::string getAsString() {
        if(fieldType != STRING_VAL) {
            throw IllegalStateException("Value is not of the type string");
        }
        return this->stringData;
    }

    void setAsInteger(const std::string &value) {
        if(!Utilities::isInteger(value)) {
            throw InvalidArgumentException("Value is not integer: " + value);
        }
        integerData = Utilities::StringToInt(value);

        this->fieldType = INTEGER_VAL;
    }

    int getAsInteger() {
        if(fieldType != INTEGER_VAL) {
            throw IllegalStateException("Value is not of the type int");
        }

        return this->integerData;
    }

    void setAsDouble(const std::string &value) {
        if(!Utilities::isDouble(value)) {
            throw InvalidArgumentException("Value is not double: " + value);
        }

        doubleData = Utilities::StringToDouble(value);

        this->fieldType = DOUBLE_VAL;
    }

    double getAsDouble() {
        if(fieldType != DOUBLE_VAL) {
            throw IllegalStateException("Value is not of the type double");
        }
        return this->doubleData;
    }

    ValueType getFieldType() {
        return this->fieldType;
    }
};


#endif //CSV_READER_FIELD_H
