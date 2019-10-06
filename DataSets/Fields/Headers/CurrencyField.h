//
// Created by Petr Flajsingr on 08/11/2018.
//

#ifndef CSV_READER_CURRENCYFIELD_H
#define CSV_READER_CURRENCYFIELD_H

#include "BaseField.h"
#include "Exceptions.h"
#include "decimal.h"
#include <string>

namespace DataSets {

/**
 * Field for currency values represented as fixed point with 2 decimals.
 */
class CurrencyField : public BaseField {
public:
  /**
   *
   * @param fieldName name of the field (column)
   * @param index index in data set
   * @param dataSet owner data set
   */
  CurrencyField(std::string_view fieldName, gsl::index index, BaseDataSet *dataSet);

  [[nodiscard]] ValueType getFieldType() const override;

  void setAsString(std::string_view newValue) override;

  [[nodiscard]] std::string getAsString() const override;

  void setAsCurrency(const Currency &newValue);

  [[nodiscard]] Currency getAsCurrency() const;

  [[nodiscard]] std::function<int8_t(const DataSetRow *, const DataSetRow *)> getCompareFunction() const override;

protected:
  void setValue(const void *data) override;

  Currency value;
};
} // namespace DataSets

#endif // CSV_READER_CURRENCYFIELD_H
