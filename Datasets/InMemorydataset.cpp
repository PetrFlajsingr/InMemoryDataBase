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

    setFieldValues(0, true);
}

void InMemorydataset::loadData() {
    if(!this->fieldTypesSet) {
        createFields(dataProvider->getColumnNames());
    }

    while(!this->dataProvider->eof()) {
        this->addRecord();
        this->dataValidity.push_back(true);

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

bool InMemorydataset::setFieldValues(unsigned long index, bool searchForward) {
    int iter = index;
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
        this->currentRecord = iter;
    }

    auto value = this->data[iter];

    if(!isFieldTypeSet()) {
        setFieldTypes(value); // NOLINT
    }

    for(int iter = 0; iter < fields.size(); iter++) {
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
            default:
                throw IllegalStateException("Internal error.");
        }
    }

    return true;
}

void InMemorydataset::first() {
    this->currentRecord = 0;
    setFieldValues(0, true);
}

void InMemorydataset::last() {
    this->currentRecord = this->data.size() - 1;
    setFieldValues(this->data.size() - 1, false);
}

void InMemorydataset::next() {
    this->currentRecord++;
    if(!this->eof()) {
        setFieldValues(this->currentRecord, true);
    }
}

void InMemorydataset::previous() {
    if(this->currentRecord == 0) {
        return;
    }
    this->currentRecord--;
    setFieldValues(this->currentRecord, false);
}

void InMemorydataset::sort(unsigned long fieldIndex, SortOrder sortOrder) {
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

void InMemorydataset::find(InMemorydataset::SearchOptions &options) {
    if(options.fieldIndexes.empty()) {
        dataValidityChanged = false;
        for(int iter = 0; iter < this->dataValidity.size(); iter++) {
            this->dataValidity[iter] = true;
        }

        this->first();

        return;
    }

    dataValidityChanged = true;
    for (auto iter = 0; iter < this->data.size(); iter++) {
        bool valid = true;

        for(auto fieldIndex : options.fieldIndexes) {
            if(!valid){
                break;
            }
            switch (this->data[iter][fieldIndex]->valueType) {
                case INTEGER_VAL:
                    if(!(std::to_string(this->data[iter][fieldIndex]->data._integer) == options.searchStrings[iter])){
                        valid = false;
                    }
                    break;
                case DOUBLE_VAL:
                    if(!(std::to_string(this->data[iter][fieldIndex]->data._double) == options.searchStrings[iter])){
                        valid = false;
                    }
                    break;
                case STRING_VAL:
                    if(!(this->data[iter][fieldIndex]->data._string == options.searchStrings[fieldIndex])){
                        valid = false;
                    }
                    break;
                default:
                    throw IllegalStateException("Internal error.");
            }
        }

        this->dataValidity[iter] = valid;
    }

    this->first();
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








