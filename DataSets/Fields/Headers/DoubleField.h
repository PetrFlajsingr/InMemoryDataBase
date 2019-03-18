//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_FIELDS_HEADERS_DOUBLEFIELD_H_
#define DATASETS_FIELDS_HEADERS_DOUBLEFIELD_H_

#include "BaseField.h"
#include "Utilities.h"
#include <string>

namespace DataSets {
/**
 * Field umoznujici pristup k datum typu double v data setu.
 */
class DoubleField : public BaseField {
 public:
  DoubleField(std::string_view fieldName,
              gsl::index index,
              BaseDataSet *dataSet);

  ValueType getFieldType() const override;

  void setAsString(std::string_view value) override;

  std::string getAsString() const override;

  void setAsDouble(double value);

  double getAsDouble() const;

  std::function<int8_t(const DataSetRow *,
                       const DataSetRow *)> getCompareFunction() const override;

 protected:
  void setValue(void *data) override;

  double data;
};
}

#endif //  DATASETS_FIELDS_HEADERS_DOUBLEFIELD_H_
