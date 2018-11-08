//
// Created by Petr Flajsingr on 08/11/2018.
//

#ifndef CSV_READER_CURRENCYFIELD_H
#define CSV_READER_CURRENCYFIELD_H

#include "BaseField.h"
#include "Exceptions.h"
#include "Decimal.h"

namespace DataSets {
class CurrencyField : BaseField {
 protected:
  void setValue(void *data) override;

  dec::decimal<2> data;
 public:
  ValueType getFieldType() override;

  void setAsString(const std::string &value) override;

  std::string getAsString() override;

  std::function<bool(DataSetRow *, DataSetRow *)> getCompareFunction(SortOrder order) override;
};
}

#endif //CSV_READER_CURRENCYFIELD_H
