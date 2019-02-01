//
// Created by Petr Flajsingr on 2019-01-27.
//

#ifndef CSV_READER_DATETIMEFIELD_H
#define CSV_READER_DATETIMEFIELD_H

#include "BaseField.h"
#include <MemoryDataSet.h>
#include <DateTimeUtils.h>

namespace DataSets {

/**
 * Trida zpristupnujici hodnoty typu DateTime v data setu.
 */
class DateTimeField : public BaseField {
 public:
  DateTimeField(const std::string &fieldName,
                BaseDataSet *dataset,
                uint64_t index);

  ValueType getFieldType() const override;

  void setAsString(const std::string &value) override;

  std::string getAsString() const override;

  void setAsDateTime(const DateTime &dateTime);

  DateTime getAsDateTime() const;

  std::function<int8_t(DataSetRow *,
                       DataSetRow *)> getCompareFunction() override;

 protected:
  void setValue(void *data) override;

  DateTime data;
};

}  // namespace DataSets

#endif //CSV_READER_DATETIMEFIELD_H
