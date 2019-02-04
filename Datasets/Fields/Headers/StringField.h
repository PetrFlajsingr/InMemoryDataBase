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
 * Field zpristupnujici hodnoty typu std::string ulozene v data setu
 */
class StringField : public BaseField {
 public:
  StringField(std::string_view fieldName,
              gsl::index index,
              BaseDataSet *dataSet);

  ValueType getFieldType() const override;

  void setAsString(std::string_view value) override;

  std::string_view getAsString() const override;

  std::function<int8_t(const DataSetRow &,
                       const DataSetRow &)> getCompareFunction() override;

 protected:
  std::string data;

  void setValue(void *data) override;
};
}

#endif //  DATASETS_FIELDS_HEADERS_STRINGFIELD_H_
