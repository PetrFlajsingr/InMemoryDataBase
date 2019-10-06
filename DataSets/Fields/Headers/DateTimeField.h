//
// Created by Petr Flajsingr on 2019-01-27.
//

#ifndef CSV_READER_DATETIMEFIELD_H
#define CSV_READER_DATETIMEFIELD_H

#include "BaseField.h"
#include <MemoryDataSet.h>

namespace DataSets {

/**
 * Field for date time values.
 */
class DateTimeField : public BaseField {
public:
  /**
   *
   * @param fieldName name of the field (column)
   * @param index index in data set
   * @param dataSet owner data set
   */
  DateTimeField(std::string_view fieldName, gsl::index index, BaseDataSet *dataSet);

  [[nodiscard]] ValueType getFieldType() const override;

  void setAsString(std::string_view newValue) override;

  [[nodiscard]] std::string getAsString() const override;

  void setAsDateTime(const DateTime &newValue);

  [[nodiscard]] DateTime getAsDateTime() const;

  [[nodiscard]] std::function<int8_t(const DataSetRow *, const DataSetRow *)> getCompareFunction() const override;

protected:
  void setValue(const void *data) override;

  DateTime value;
};

} // namespace DataSets

#endif // CSV_READER_DATETIMEFIELD_H
