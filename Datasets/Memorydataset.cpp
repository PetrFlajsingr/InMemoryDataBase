//
// Created by Petr Flajsingr on 25/08/2018.
//

#include <iostream>
#include "Memorydataset.h"
#include "../Misc/Exceptions.h"
#include "../Misc/Utilities.h"
#include "Fields/IntegerField.h"
#include "Fields/DoubleField.h"
#include "Fields/StringField.h"

void Memorydataset::setDataProvider(IDataProvider *provider) {
    this->dataProvider = provider;
}

void Memorydataset::open() {
    if(isOpen) {
        throw IllegalStateException("Dataset is already opened");
    }

    if(this->dataProvider == nullptr) {
        throw IllegalStateException("Data provider has not been set.");
    }

    this->loadData();

    this->isOpen = true;

    delete this->dataProvider;

    setFieldValues(0, true);
}

void Memorydataset::loadData() {
    if(!this->fieldTypesSet) {
        createFields(dataProvider->getColumnNames(), {});
    }

    while(!this->dataProvider->eof()) {
        this->addRecord();
        this->dataValidity.push_back(true);

        this->dataProvider->next();
    }
}

void Memorydataset::createFields(std::vector<std::string> columns, std::vector<ValueType> types) {
    if(columns.size() != types.size()) {
        throw InvalidArgumentException("Amount of columns and types must match.");
    }
    size_t iter = 0;
    for(const auto &col : columns) {
        IField* newField = nullptr;
        switch(types[iter]){
            case INTEGER_VAL: {
                newField = new IntegerField(col, this, iter);
                break;
            }
            case DOUBLE_VAL: {
                newField = new DoubleField(col, this, iter);
                break;
            }
            case STRING_VAL: {
                newField = new StringField(col, this, iter);
                break;
            }
            default:
                throw IllegalStateException("Internal error.");
        }
        fields.push_back(newField);
        iter++;
    }
}

void Memorydataset::addRecord() {
    auto record = this->dataProvider->getRow();

    std::vector<DataContainer*> newRecord;
    size_t iter = 0;
    for(const auto &part : record) {
        auto dataContainer = new DataContainer();
        switch(fields[iter]->getFieldType()){
            case INTEGER_VAL:
                dataContainer->valueType = INTEGER_VAL;
                dataContainer->data._integer = Utilities::StringToInt(part);
                break;
            case DOUBLE_VAL:
                dataContainer->valueType = DOUBLE_VAL;
                dataContainer->data._double = Utilities::StringToDouble(part);
                break;
            case STRING_VAL:
                dataContainer->valueType = STRING_VAL;
                dataContainer->data._string = strdup(part.c_str());
                break;
            default:
                throw IllegalStateException("Internal error.");
        }
        iter++;

        newRecord.push_back(dataContainer);
    }

    this->data.push_back(newRecord);
}

void Memorydataset::close() {
    this->dataProvider = nullptr;

    this->isOpen = false;

    if(!this->data.empty()) {
        for(auto level1 : this->data) {
            for(auto level2 : level1) {
                delete level2;
            }
            level1.clear();
        }
        this->data.clear();
    }
}


bool Memorydataset::setFieldValues(unsigned long index, bool searchForward) {
    long iter = index;
    if(dataValidityChanged) {
        bool found = false;

        if(searchForward) {
            for (iter = index; iter < this->dataValidity.size(); iter++) {
                if (this->dataValidity[iter]) {
                    found = true;
                    break;
                }
            }
        } else {
            for (iter = index; iter >= 0; iter--) {
                if (this->dataValidity[iter]) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            if(searchForward) {
                this->currentRecord = this->data.size();
            } else {
                this->currentRecord = 0;
            }
            return false;
        }
        this->currentRecord = static_cast<unsigned long>(iter);
    }

    auto value = this->data[iter];


    for(int i = 0; i < fields.size(); i++) {
        switch(fields[i]->getFieldType()) {
            case INTEGER_VAL:
                this->setFieldData(fields[i], reinterpret_cast<u_int8_t *>(&value[i]->data._integer));
                break;
            case DOUBLE_VAL:
                this->setFieldData(fields[i], reinterpret_cast<u_int8_t *>(&value[i]->data._double));
                break;
            case STRING_VAL:
                this->setFieldData(fields[i], reinterpret_cast<u_int8_t *>(value[i]->data._string));
                break;
            default:
                throw IllegalStateException("Internal error.");
        }
    }

    return true;
}

void Memorydataset::first() {
    this->currentRecord = 0;
    setFieldValues(0, true);
}

void Memorydataset::last() {
    this->currentRecord = this->data.size() - 1;
    setFieldValues(this->data.size() - 1, false);
}

void Memorydataset::next() {
    this->currentRecord++;
    if(!this->eof()) {
        setFieldValues(this->currentRecord, true);
    }
}

void Memorydataset::previous() {
    if(this->currentRecord == 0) {
        return;
    }
    this->currentRecord--;
    setFieldValues(this->currentRecord, false);
}

void Memorydataset::sort(unsigned long fieldIndex, SortOrder sortOrder) {
    if(fieldIndex >= this->getFieldNames().size()) {
        throw InvalidArgumentException("Field index is out of bounds");
    }

    if(sortOrder == ASCENDING) {
        std::sort(this->data.begin(),
                  this->data.end(),
                  [&fieldIndex](const std::vector<DataContainer *> &a,
                                const std::vector<DataContainer *> &b) {
                      switch (a[fieldIndex]->valueType) {
                          case INTEGER_VAL:
                              return a[fieldIndex]->data._integer < b[fieldIndex]->data._integer;
                          case DOUBLE_VAL:
                              return a[fieldIndex]->data._double < b[fieldIndex]->data._double;
                          case STRING_VAL:
                              return std::strcmp(a[fieldIndex]->data._string,
                                                 b[fieldIndex]->data._string) < 0;
                          default:
                              throw IllegalStateException("Internal error.");
                      }
                  });
    } else {
        std::sort(this->data.begin(),
                  this->data.end(),
                  [&fieldIndex](const std::vector<DataContainer *> &a,
                                const std::vector<DataContainer *> &b) {
                      switch (a[fieldIndex]->valueType) {
                          case INTEGER_VAL:
                              return a[fieldIndex]->data._integer > b[fieldIndex]->data._integer;
                          case DOUBLE_VAL:
                              return a[fieldIndex]->data._double > b[fieldIndex]->data._double;
                          case STRING_VAL:
                              return std::strcmp(a[fieldIndex]->data._string,
                                                 b[fieldIndex]->data._string) > 0;
                          default:
                              throw IllegalStateException("Internal error.");
                      }
                  });
    }

    this->first();
}

void Memorydataset::find(Memorydataset::SearchOptions &options) {
    if(options.fieldIndexes.empty()) {
        dataValidityChanged = false;
        for(int iter = 0; iter < this->dataValidity.size(); iter++) {
            this->dataValidity[iter] = true;
        }

        this->first();

        return;
    }

    dataValidityChanged = true;
    size_t optionCounter;
    for (auto iter = 0; iter < this->data.size(); iter++) {
        bool valid = true;

        optionCounter = 0;
        for(auto fieldIndex : options.fieldIndexes) {
            if(!valid){
                break;
            }
            switch (this->data[iter][fieldIndex]->valueType) {
                case INTEGER_VAL:
                    if(!(std::to_string(this->data[iter][fieldIndex]->data._integer) == options.searchStrings[optionCounter])){
                        valid = false;
                    }
                    break;
                case DOUBLE_VAL:
                    if(!(std::to_string(this->data[iter][fieldIndex]->data._double) == options.searchStrings[optionCounter])){
                        valid = false;
                    }
                    break;
                case STRING_VAL:
                    if(std::strcmp(this->data[iter][fieldIndex]->data._string,
                                   options.searchStrings[optionCounter].c_str()) != 0){
                        valid = false;
                    }
                    break;
                default:
                    throw IllegalStateException("Internal error.");
            }
            optionCounter++;
        }

        this->dataValidity[iter] = valid;
    }

    this->first();
}

IField * Memorydataset::fieldByName(const std::string &name) {
    for(auto &field : fields) {
        if(field->getFieldName() == name) {
            return field;
        }
    }

    throw InvalidArgumentException(("There's no field named: " + name).c_str());
}

IField * Memorydataset::fieldByIndex(unsigned long index) {
    return fields.at(index);
}

bool Memorydataset::eof() {
    return this->currentRecord >= this->data.size();

}

Memorydataset::~Memorydataset() {
    this->close();
}

void Memorydataset::setFieldTypes(std::vector<ValueType> types) {
    fieldTypesSet = true;

    createFields(this->dataProvider->getColumnNames(), types);
}

void Memorydataset::setData(u_int8_t *data, unsigned long index, ValueType type) {
    switch(type) {
        case INTEGER_VAL:
            this->data[currentRecord][index]->data._integer = *data;
            break;
        case DOUBLE_VAL:
            this->data[currentRecord][index]->data._double = *data;
            break;
        case STRING_VAL:
            if(this->data[currentRecord][index]->data._string != nullptr) {
                delete [] this->data[currentRecord][index]->data._string;
            }
            this->data[currentRecord][index]->data._string = reinterpret_cast<char *>(data);
            break;
        default:
            throw IllegalStateException("Invalid value type.");
    }

}








