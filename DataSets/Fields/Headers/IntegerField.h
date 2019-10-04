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
 * Field for integer values.
 */
class IntegerField : public BaseField {
 public:
  /**
   *
   * @param fieldName name of the field (column)
   * @param index index in data set
   * @param dataSet owner data set
   */
  IntegerField(std::string_view fieldName, gsl::index index, BaseDataSet *dataSet);
  ValueType getFieldType() const override;

    void setAsString(std::string_view newValue) override;
  std::string getAsString() const override;

    void setAsInteger(int newValue);
  int getAsInteger() const;

  std::function<int8_t(const DataSetRow *, const DataSetRow *)> getCompareFunction() const override;

 protected:
    void setValue(const void *data) override;

    int value;
};
}

#endif //  DATASETS_FIELDS_HEADERS_INTEGERFIELD_H_
