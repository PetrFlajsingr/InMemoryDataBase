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
class CurrencyField : public BaseField {
 protected:
  void setValue(void *data) override;

  Currency data;
 public:
  CurrencyField(const std::string& fieldName,
                BaseDataSet *dataset,
                uint64_t index);

  ValueType getFieldType() override;

  void setAsString(const std::string &value) override;

  std::string getAsString() override;

  void setAsCurrency(Currency& value);

  Currency getAsCurrency();

  std::function<bool(DataSetRow *, DataSetRow *)> getCompareFunction(SortOrder order) override;
};
}

#endif //CSV_READER_CURRENCYFIELD_H
