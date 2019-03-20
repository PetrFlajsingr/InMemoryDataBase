//
// Created by Petr Flajsingr on 2019-01-27.
//

#ifndef CSV_READER_DATETIMEFIELD_H
#define CSV_READER_DATETIMEFIELD_H

#include "BaseField.h"
#include <MemoryDataSet.h>

namespace DataSets {

/**
 * Trida zpristupnujici hodnoty typu DateTime v data setu.
 */
class DateTimeField : public BaseField {
 public:
  DateTimeField(std::string_view fieldName,
                gsl::index index,
                BaseDataSet *dataSet);

  ValueType getFieldType() const override;

  void setAsString(std::string_view value) override;

  std::string getAsString() const override;

  void setAsDateTime(const DateTime &dateTime);

  DateTime getAsDateTime() const;

  std::function<int8_t(const DataSetRow *,
                       const DataSetRow *)> getCompareFunction() const override;

 protected:
  void setValue(void *data) override;

  DateTime data;
};

}  // namespace DataSets

#endif //CSV_READER_DATETIMEFIELD_H
