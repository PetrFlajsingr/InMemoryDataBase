//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef CSV_READER_INTEGERFIELD_H
#define CSV_READER_INTEGERFIELD_H


#include "../../Interfaces/IField.h"

class IntegerField : public IField {
protected:
    void setValue(void *data) override;

    int data;
public:
    IntegerField(const std::string &fieldName, IDataset *dataset, unsigned long index);

    ValueType getFieldType() override;

    void setAsString(const std::string &value) override;

    std::string getAsString() override;

    void setAsInteger(int value);

    int getAsInteger();
};


#endif //CSV_READER_INTEGERFIELD_H
