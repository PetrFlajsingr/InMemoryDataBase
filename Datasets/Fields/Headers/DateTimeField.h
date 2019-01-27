//
// Created by Petr Flajsingr on 2019-01-27.
//

#ifndef CSV_READER_DATETIMEFIELD_H
#define CSV_READER_DATETIMEFIELD_H

#include "BaseField.h"
#include <MemoryDataSet.h>
#include <DateTimeUtils.h>

namespace DataSets {

class DateTimeField : public BaseField {
 protected:
  void setValue(void *data) override;

  DateTime data;

 public:
  ValueType getFieldType() override;

  void setAsString(const std::string &value) override;

  std::string getAsString() override;

  void setAsDateTime(const DateTime &dateTime);

  DateTime getAsDateTime();

  std::function<int8_t(DataSetRow *, DataSetRow *)> getCompareFunction() override;
};

}  // namespace DataSets

#endif //CSV_READER_DATETIMEFIELD_H
