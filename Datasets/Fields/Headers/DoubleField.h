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
 * Field umoznujici pristup k datum typu Double.
 */
class DoubleField : public BaseField {
 protected:
  void setValue(void *data) override;

  double data;
 public:
  DoubleField(const std::string &fieldName,
              BaseDataSet *dataset,
              uint64_t index);

  ValueType getFieldType() override;

  void setAsString(const std::string &value) override;

  std::string getAsString() override;

  void setAsDouble(double value);

  double getAsDouble();

  std::function<bool(DataSetRow *, DataSetRow *)> getCompareFunction(SortOrder order) override;
};
}

#endif //  DATASETS_FIELDS_HEADERS_DOUBLEFIELD_H_
