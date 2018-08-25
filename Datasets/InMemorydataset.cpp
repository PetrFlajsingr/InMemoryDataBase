//
// Created by Petr Flajsingr on 25/08/2018.
//

#include "InMemorydataset.h"

void InMemorydataset::open(IDataProvider *provider) {
    if(isOpen) {
        throw IllegalStateException("Dataset is already opened");
    }

    this->dataProvider = provider;

    this->loadData();

    this->isOpen = true;

    delete this->dataProvider;
}

void InMemorydataset::loadData() {
    createFields(dataProvider->getColumnNames());

    while(!this->dataProvider->eof()) {
        this->addRecord();

        this->dataProvider->next();
    }
}

void InMemorydataset::createFields(std::vector<std::string> columns) {
    for(const auto &col : columns) {
        Field newField(col);
        fields.push_back(newField);
    }
}

void InMemorydataset::addRecord() {
    auto record = this->dataProvider->getRow();

    std::vector<DataContainer> newRecord;
    for(const auto &part : record) {
        DataContainer dataContainer{};
        switch(Utilities::getType(part)){
            case INTEGER_VAL:
                dataContainer.valueType = INTEGER_VAL;
                dataContainer.data._integer = Utilities::StringToInt(part);
                break;
            case DOUBLE_VAL:
                dataContainer.valueType = DOUBLE_VAL;
                dataContainer.data._double = Utilities::StringToDouble(part);
                break;
            case STRING_VAL:
                dataContainer.valueType = STRING_VAL;
                dataContainer.data._string = strdup(part.c_str());
                break;
            default:
                throw IllegalStateException("Internal error.");
        }

        newRecord.push_back(dataContainer);
    }

    this->data.push_back(newRecord);
}

void InMemorydataset::close() {
    this->dataProvider = nullptr;

    this->isOpen = false;
}

void InMemorydataset::setFieldTypes(std::vector<InMemorydataset::DataContainer> value) {
    for(int iter = 0; iter < fields.size(); iter++) {
        fields[iter].setFieldType(value[iter].valueType);
    }
}

void InMemorydataset::setFieldValues(std::vector<InMemorydataset::DataContainer> value) {
    if(!isFieldTypeSet()) {
        setFieldTypes(value); // NOLINT
        return;
    }

    for(int iter = 0; iter < fields.size(); iter++) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
        switch(fields[iter].getFieldType()) {
            case INTEGER_VAL:
                fields[iter].setFromInteger(value[iter].data._integer);
                break;
            case DOUBLE_VAL:
                fields[iter].setFromDouble(value[iter].data._double);
                break;
            case STRING_VAL:
                fields[iter].setAsString(value[iter].data._string);
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
    setFieldValues(this->data[this->currentRecord]);
}

void InMemorydataset::previous() {
    this->currentRecord--;
    setFieldValues(this->data[this->currentRecord]);
}

void InMemorydataset::sort(InMemorydataset::SortOptions &options) {

}

void InMemorydataset::find(InMemorydataset::SearchOptions &options) {

}

const Field * InMemorydataset::fieldByName(const std::string &name) {
    for(const auto &field : fields) {
        if(field.fieldName == name) {
            return &field;
        }
    }

    throw InvalidArgumentException(("There's no field named: " + name).c_str());
}

const Field * InMemorydataset::fieldByIndex(unsigned long index) {
    return &fields.at(index);
}

bool InMemorydataset::eof() {
    return this->currentRecord >= this->data.size() - 1;

}






