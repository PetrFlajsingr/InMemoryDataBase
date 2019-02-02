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
                BaseDataSet *dataset,
                uint64_t index);

  ValueType getFieldType() const override;

  void setAsString(std::string_view value) override;

  std::string getAsString() const override;

  void setAsCurrency(Currency &value);

  Currency getAsCurrency() const;

  std::function<int8_t(DataSetRow *,
                       DataSetRow *)> getCompareFunction() override;

 protected:
  void setValue(void *data) override;

  Currency data;
};
}

#endif //CSV_READER_CURRENCYFIELD_H
