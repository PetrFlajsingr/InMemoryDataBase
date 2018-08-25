//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef CSV_READER_IDATASET_H
#define CSV_READER_IDATASET_H

#include <string>
#include <vector>
#include <cstring>
#include "../Datasets/Field.h"
#include "IDataProvider.h"

class IDataset {
protected:
    std::vector<Field> fields;
public:
    virtual void open(IDataProvider* dataProvider)= 0;

    virtual void close()= 0;

    virtual void first()= 0;

    virtual void last()= 0;

    virtual void next()= 0;

    virtual void previous()= 0;

    virtual bool eof()= 0;

    virtual const Field * fieldByName(const std::string& name)= 0;

    virtual const Field * fieldByIndex(unsigned long index)= 0;

    virtual std::vector<std::string> getFieldNames()= 0;

    virtual ~IDataset() = default;
};

#endif //CSV_READER_IDATASET_H
