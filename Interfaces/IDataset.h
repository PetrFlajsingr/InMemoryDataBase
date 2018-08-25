//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef CSV_READER_IDATASET_H
#define CSV_READER_IDATASET_H

#include <string>
#include <vector>
#include "../Datasets/Field.h"

class IDataset {
public:
    virtual void loadData()= 0;

    virtual void first()= 0;

    virtual void last()= 0;

    virtual void next()= 0;

    virtual void previous()= 0;

    virtual Field fieldByName(const std::string& name)= 0;
};

#endif //CSV_READER_IDATASET_H
