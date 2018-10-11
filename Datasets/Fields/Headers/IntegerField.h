//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_FIELDS_HEADERS_INTEGERFIELD_H_
#define DATASETS_FIELDS_HEADERS_INTEGERFIELD_H_

#include <string>
#include "IField.h"

class IntegerField : public IField {
 protected:
    void setValue(void *data) override;

    int data;
 public:

    IntegerField(const std::string &fieldName,
            IDataSet *dataset,
            uint64_t index);

    ValueType getFieldType() override;

    void setAsString(const std::string &value) override;

    std::string getAsString() override;

    void setAsInteger(int value);

    int getAsInteger();
};


#endif //  DATASETS_FIELDS_HEADERS_INTEGERFIELD_H_
