//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef CSV_READER_FIELDBASE_H
#define CSV_READER_FIELDBASE_H

#include <string>
#include <utility>
#include "../Misc/Types.h"
#include "IDatasetPre.h"

class IField{
protected:
    std::string fieldName;

    unsigned long index;
public:
    const std::string &getFieldName() const {
        return fieldName;
    }

protected:
    friend class IDataset;

    IDataset* dataset;

    virtual void setValue(u_int8_t * data)= 0;

    void setDatasetData(u_int8_t * data, ValueType type);

public:
    explicit IField(const std::string &fieldName,
                            IDataset* dataset,
                            unsigned long index) : fieldName(std::move(fieldName)),
                                                   dataset(dataset),
                                                   index(index){}

    virtual ~IField() = default;

    virtual ValueType getFieldType()= 0;

    virtual void setAsString(const std::string& value)= 0;

    virtual std::string getAsString()= 0;
};

#endif //CSV_READER_FIELDBASE_H
