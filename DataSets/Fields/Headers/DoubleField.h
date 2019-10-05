//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_FIELDS_HEADERS_DOUBLEFIELD_H_
#define DATASETS_FIELDS_HEADERS_DOUBLEFIELD_H_

#include "BaseField.h"
#include <string>

namespace DataSets {
/**
 * Field for double values.
 */
class DoubleField : public BaseField {
 public:
  /**
   *
   * @param fieldName name of the field (column)
   * @param index index in data set
   * @param dataSet owner data set
   */
  DoubleField(std::string_view fieldName, gsl::index index, BaseDataSet *dataSet);

    [[nodiscard]] ValueType getFieldType() const override;

    void setAsString(std::string_view newValue) override;

    [[nodiscard]] std::string getAsString() const override;

    void setAsDouble(double newValue);

    [[nodiscard]] double getAsDouble() const;

    [[nodiscard]] std::function<int8_t(const DataSetRow *, const DataSetRow *)> getCompareFunction() const override;

 protected:
    void setValue(const void *newValue) override;

    double value;
};
}

#endif //  DATASETS_FIELDS_HEADERS_DOUBLEFIELD_H_
