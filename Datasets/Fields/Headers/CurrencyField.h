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
typedef dec::decimal<2> Currency;

class CurrencyField : BaseField {
 protected:
  void setValue(void *data) override;

  Currency data;
 public:
  ValueType getFieldType() override;

  void setAsString(const std::string &value) override;

  std::string getAsString() override;

  void setAsCurrency(Currency& value);

  Currency getAsCurrency();

  std::function<bool(DataSetRow *, DataSetRow *)> getCompareFunction(SortOrder order) override;
};
}

#endif //CSV_READER_CURRENCYFIELD_H
