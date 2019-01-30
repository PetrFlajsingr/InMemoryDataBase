//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_FIELDS_HEADERS_INTEGERFIELD_H_
#define DATASETS_FIELDS_HEADERS_INTEGERFIELD_H_

#include <string>
#include "BaseField.h"
#include "Utilities.h"

namespace DataSets {

/**
 * Field zpristupnujici hodnoty typu int v data setu.
 */
class IntegerField : public BaseField {
 protected:
  void setValue(void *data) override;

  int data;

 public:
  IntegerField(const std::string &fieldName,
               BaseDataSet *dataset,
               uint64_t index);

  ValueType getFieldType() const override;

  void setAsString(const std::string &value) override;

  std::string getAsString() const override;

  void setAsInteger(int value);

  int getAsInteger() const;

  std::function<int8_t(DataSetRow *,
                       DataSetRow *)> getCompareFunction() override;
};
}

#endif //  DATASETS_FIELDS_HEADERS_INTEGERFIELD_H_
