//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef CSV_READER_STRINGFIELD_H
#define CSV_READER_STRINGFIELD_H


#include "IField.h"

class StringField : public IField {
protected:
    std::string data;

    void setValue(void *data) override;
public:
    StringField(const std::string &fieldName, IDataset *dataset, unsigned long index);

    ValueType getFieldType() override;

    void setAsString(const std::string& value) override;

    std::string getAsString() override;
};


#endif //CSV_READER_STRINGFIELD_H
