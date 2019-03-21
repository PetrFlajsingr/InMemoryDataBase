//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_FIELDS_HEADERS_STRINGFIELD_H_
#define DATASETS_FIELDS_HEADERS_STRINGFIELD_H_

#include "BaseField.h"
#include "Utilities.h"
#include <string>

namespace DataSets {

/**
 * Field for std::string values.
 */
class StringField : public BaseField {
 public:
  /**
   *
   * @param fieldName name of the field (column)
   * @param index index in data set
   * @param dataSet owner data set
   */
  StringField(std::string_view fieldName, gsl::index index, BaseDataSet *dataSet);
  ValueType getFieldType() const override;
  void setAsString(std::string_view value) override;
  std::string getAsString() const override;

  std::function<int8_t(const DataSetRow *, const DataSetRow *)> getCompareFunction() const override;

 protected:
  std::string data;
  void setValue(void *data) override;
};
}

#endif //  DATASETS_FIELDS_HEADERS_STRINGFIELD_H_
