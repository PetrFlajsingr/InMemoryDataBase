//
// Created by Petr Flajsingr on 08/11/2018.
//

#ifndef CSV_READER_CURRENCYFIELD_H
#define CSV_READER_CURRENCYFIELD_H

#include <string>
#include "BaseField.h"
#include "Exceptions.h"
#include "decimal.h"

namespace DataSets {

/**
 * Field zpristupnujici hodnoty typu Currency v data setu.
 */
class CurrencyField : public BaseField {
 public:
  CurrencyField(std::string_view fieldName,
                gsl::index index,
                BaseDataSet *dataSet);

  ValueType getFieldType() const override;

  void setAsString(std::string_view value) override;

  std::string_view getAsString() const override;

  void setAsCurrency(const Currency &value);

  Currency getAsCurrency() const;

  std::function<int8_t(const DataSetRow &,
                       const DataSetRow &)> getCompareFunction() override;

 protected:
  void setValue(void *data) override;

  Currency data;
};
}

#endif //CSV_READER_CURRENCYFIELD_H
