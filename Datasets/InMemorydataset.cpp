//
// Created by Petr Flajsingr on 25/08/2018.
//

#include <iostream>
#include "InMemorydataset.h"

void InMemorydataset::setDataProvider(IDataProvider *provider) {
    this->dataProvider = provider;
}

void InMemorydataset::open() {
    if(isOpen) {
        throw IllegalStateException("Dataset is already opened");
    }

    if(this->dataProvider == nullptr) {
        throw IllegalStateException("Data provider has not been set.");
    }

    this->loadData();

    this->isOpen = true;

    delete this->dataProvider;

    setFieldValues(this->data[0]);
}

void InMemorydataset::loadData() {
    if(!this->fieldTypesSet) {
        createFields(dataProvider->getColumnNames());
    }

    while(!this->dataProvider->eof()) {
        this->addRecord();

        this->dataProvider->next();
    }
}

void InMemorydataset::createFields(std::vector<std::string> columns, std::vector<ValueType> types) {
    size_t iter = 0;
    for(const auto &col : columns) {
        Field newField(col);
        if(!types.empty()) {
            newField.setFieldType(types[iter]);
        }
        fields.push_back(newField);
        iter++;
    }
}

void InMemorydataset::addRecord() {
    auto record = this->dataProvider->getRow();

    std::vector<DataContainer*> newRecord;
    size_t iter = 0;
    for(const auto &part : record) {
        auto dataContainer = new DataContainer();
        switch(fields[iter].getFieldType()){
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

void InMemorydataset::close() {
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

void InMemorydataset::setFieldTypes(std::vector<InMemorydataset::DataContainer*> value) {
    for(int iter = 0; iter < fields.size(); iter++) {
        fields[iter].setFieldType(value[iter]->valueType);
    }
}

void InMemorydataset::setFieldValues(std::vector<InMemorydataset::DataContainer*> value) {
    if(!isFieldTypeSet()) {
        setFieldTypes(value); // NOLINT
        return;
    }

    for(int iter = 0; iter < fields.size(); iter++) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
        switch(fields[iter].getFieldType()) {
            case INTEGER_VAL:
                fields[iter].setFromInteger(value[iter]->data._integer);
                break;
            case DOUBLE_VAL:
                fields[iter].setFromDouble(value[iter]->data._double);
                break;
            case STRING_VAL:
                fields[iter].setAsString(value[iter]->data._string);
                break;
        }
#pragma clang diagnostic pop
    }
}

void InMemorydataset::first() {
    this->currentRecord = 0;
    setFieldValues(this->data[0]);
}

void InMemorydataset::last() {
    this->currentRecord = this->data.size() - 1;
    setFieldValues(this->data[this->data.size() - 1]);
}

void InMemorydataset::next() {
    this->currentRecord++;
    if(!this->eof()) {
        setFieldValues(this->data[this->currentRecord]);
    }
}

void InMemorydataset::previous() {
    if(this->currentRecord == 0) {
        return;
    }
    this->currentRecord--;
    setFieldValues(this->data[this->currentRecord]);
}

void InMemorydataset::sort(InMemorydataset::SortOptions &options) {
    //TODO
}

void InMemorydataset::find(InMemorydataset::SearchOptions &options) {
    //TODO
}

Field * InMemorydataset::fieldByName(const std::string &name) {
    for(auto &field : fields) {
        if(field.fieldName == name) {
            return &field;
        }
    }

    throw InvalidArgumentException(("There's no field named: " + name).c_str());
}

Field * InMemorydataset::fieldByIndex(unsigned long index) {
    return &fields.at(index);
}

bool InMemorydataset::eof() {
    return this->currentRecord >= this->data.size();

}

InMemorydataset::~InMemorydataset() {
    this->close();
}

void InMemorydataset::setFieldTypes(std::vector<ValueType> types) {
    fieldTypesSet = true;

    createFields(this->dataProvider->getColumnNames(), types);
}








