//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_FIELDS_HEADERS_STRINGFIELD_H_
#define DATASETS_FIELDS_HEADERS_STRINGFIELD_H_


#include "IField.h"
#include <string>

class StringField : public IField {
 protected:
    std::string data;

    void setValue(void *data) override;
 public:
    StringField(const std::string &fieldName,
            IDataSet *dataset,
            uint64_t index);

    ValueType getFieldType() override;

    void setAsString(const std::string& value) override;

    std::string getAsString() override;
};


#endif //  DATASETS_FIELDS_HEADERS_STRINGFIELD_H_
