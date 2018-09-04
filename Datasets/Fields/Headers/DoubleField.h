//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef CSV_READER_DOUBLEFIELD_H
#define CSV_READER_DOUBLEFIELD_H


#include "IField.h"

class DoubleField : public IField{
protected:
    void setValue(void *data) override;

    double data;
public:
    DoubleField(const std::string &fieldName, IDataset *dataset, unsigned long index);

    ValueType getFieldType() override;

    void setAsString(const std::string &value) override;

    std::string getAsString() override;

    void setAsDouble(double value);

    double getAsDouble();
};


#endif //CSV_READER_DOUBLEFIELD_H
