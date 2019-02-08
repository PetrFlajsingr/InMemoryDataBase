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
 public:
  IntegerField(std::string_view fieldName,
               gsl::index index,
               BaseDataSet *dataSet);

  ValueType getFieldType() const override;

  void setAsString(std::string_view value) override;

  std::string_view getAsString() const override;

  void setAsInteger(int value);

  int getAsInteger() const;

  std::function<int8_t(const DataSetRow &,
                       const DataSetRow &)> getCompareFunction() const override;

 protected:
  void setValue(void *data) override;

  int data;
};
}

#endif //  DATASETS_FIELDS_HEADERS_INTEGERFIELD_H_
