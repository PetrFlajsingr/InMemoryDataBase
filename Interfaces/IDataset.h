//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef CSV_READER_IDATASET_H
#define CSV_READER_IDATASET_H

#include <string>
#include <vector>
#include <cstring>
#include "IFieldPre.h"
#include "IDataProvider.h"
#include "../Misc/Types.h"

class IDataset {
protected:
    friend class IField;

    std::vector<IField*> fields;

    void setFieldData(IField *field, u_int8_t *data);

    virtual void setData(u_int8_t * data, unsigned long index, ValueType type)= 0;
public:
    virtual void open()= 0;

    virtual void setDataProvider(IDataProvider* provider)= 0;

    virtual void close()= 0;

    virtual void first()= 0;

    virtual void last()= 0;

    virtual void next()= 0;

    virtual void previous()= 0;

    virtual bool eof()= 0;

    virtual IField * fieldByName(const std::string& name)= 0;

    virtual IField * fieldByIndex(unsigned long index)= 0;

    virtual void setFieldTypes(std::vector<ValueType> types)= 0;

    virtual std::vector<std::string> getFieldNames()= 0;

    virtual ~IDataset() {
        for(auto tmp : this->fields) {
            delete tmp;
        }
    };
};

#endif //CSV_READER_IDATASET_H
