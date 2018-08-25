//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef CSV_READER_FIELD_H
#define CSV_READER_FIELD_H


#include <string>
#include <utility>
#include "../Misc/Exceptions.h"
#include "../Misc/Utilities.h"

/**
 * Trida uchovavajici zaznamy datasetu pro iteraci mezi nimi.
 * Predpoklada se prubezne nastaveni pri jakekoliv zmene zaznamu.
 */
class Field {
private:
    ValueType fieldType = NONE;

    std::string stringData;

    int integerData;

    double doubleData;

    bool typeSet = false;
public:
    explicit Field(std::string name);

    ~Field();

    const std::string fieldName;

    /**
     * Nastaveni hodnoty pole. Typ je odvozen automaticky.
     * @param value libovolna hodnota
     */
    void setValue(std::string value);

    /**
     * Nastaveni hodnoty jako string
     * @param value
     */
    void setAsString(std::string value) {
        if(typeSet && fieldType != STRING_VAL) {
            throw IllegalStateException(("Field type has already been set to: " + std::to_string(int(fieldType))).c_str());
        }

        this->stringData = std::move(value);

        this->typeSet = true;
    }

    /**
     * Navrat hodnoty jako string
     * @return
     */
    std::string getAsString() {
        if(fieldType == NONE) {
            throw IllegalStateException("Field type has not been set");
        }
        switch(fieldType){
            case NONE:
                throw IllegalStateException("Value is not of the type string");
            case INTEGER_VAL:
                return std::to_string(this->integerData);
            case DOUBLE_VAL:
                return std::to_string(this->doubleData);
            case STRING_VAL:
                return this->stringData;
            default:
                throw IllegalStateException(("Interni chyba, fieldType: " + std::to_string(int(fieldType))).c_str());
        }
    }

    void setAsInteger(const std::string &value) {
        if(typeSet && fieldType != INTEGER_VAL) {
            throw IllegalStateException(("Field type has already been set to: " + std::to_string(int(fieldType))).c_str());
        }

        if(!Utilities::isInteger(value)) {
            throw InvalidArgumentException(("Value is not integer: " + value).c_str());
        }
        integerData = Utilities::StringToInt(value);

        this->typeSet = true;
    }

    void setFromInteger(int value) {
        this->integerData = value;
    }

    int getAsInteger() {
        if(fieldType != INTEGER_VAL) {
            throw IllegalStateException("Value is not of the type int");
        }

        return this->integerData;
    }

    void setAsDouble(const std::string &value) {
        if(typeSet && fieldType != DOUBLE_VAL) {
            throw IllegalStateException(("Field type has already been set to: " + std::to_string(int(fieldType))).c_str());
        }

        if(!Utilities::isDouble(value)) {
            throw InvalidArgumentException(("Value is not double: " + value).c_str());
        }

        doubleData = Utilities::StringToDouble(value);

        this->typeSet = true;
    }

    void setFromDouble(double value) {
        this->doubleData = value;
    }

    double getAsDouble() {
        if(fieldType != DOUBLE_VAL) {
            throw IllegalStateException("Value is not of the type double");
        }
        return this->doubleData;
    }

    void setFieldType(ValueType type) {
        this->fieldType = type;
    }

    ValueType getFieldType() {
        return this->fieldType;
    }
};


#endif //CSV_READER_FIELD_H
