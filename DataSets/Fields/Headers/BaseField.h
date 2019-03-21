//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_FIELDS_HEADERS_BASEFIELD_H_
#define DATASETS_FIELDS_HEADERS_BASEFIELD_H_

#include <string>
#include <utility>
#include <functional>
#include "Types.h"
#include "Exceptions.h"

namespace DataSets {
class BaseDataSet;

/**
 * Base class for fields.
 * Field is an interface to provide data from dataset while not providing the same
 * memory location.
 */
class BaseField {
 public:
  /**
   *
   * @param fieldName name of the field (column)
   * @param dataset dataset this field provides data from
   * @param index column index in dataset
   */
  explicit BaseField(std::string_view fieldName, gsl::index index, BaseDataSet *dataSet)
      : dataSet(dataSet), fieldName(fieldName), index(index) {}
  virtual ~BaseField() = default;
  /**
   *
   * @return type of data provided by this
   */
  virtual ValueType getFieldType() const = 0;
  /**
   * Set value as string - convert to inner value and set it in data set
   * @param value value to set
   */
  virtual void setAsString(std::string_view value) = 0;
  /**
   * Convert inner value to string
   * @return value as string
   */
  virtual std::string getAsString() const = 0;
  /**
   *
   * @return index of a field in data set
   */
  gsl::index getIndex() const {
    return index;
  }
  /**
   *
   * @return Name of field/column
   */
  std::string_view getName() const {
    return fieldName;
  }
  /**
   * Change field name
   * @param name new name
   */
  void setName(std::string_view name) {
    fieldName = name;
  }
  /**
   * Compare function for MemoryDataSet. Used for MemoryDataSet::Sort
   * @return compare function returning
   *    0   -   equal
   *    1   -   first is greater
   *    -1  -   second is greater
   */
  virtual std::function<int8_t(const DataSetRow *, const DataSetRow *)> getCompareFunction() const = 0;
  /**
   * Index conversion for MemoryViewDataSet.
   * @param field field whose index we need to convert
   * @return [a, b] where a is an index of table and b is an index of a column
   */
  static std::pair<gsl::index, gsl::index> convertIndex(const BaseField &field);
  static const gsl::index maskTableShift = 16;
  static const gsl::index maskTableIndex = 0xFF0000;
  static const gsl::index maskColumnIndex = 0x00FFFF;

 protected:
  friend class BaseDataSet; //< to allow for direct memory access
  BaseDataSet *dataSet;
  // TODO: predavat pointer na DataContainer
  /**
   * Set inner field value.
   * @param data pointer to data
   */
  virtual void setValue(void *data) = 0;
  // TODO: predavat pointer na DataContainer
  /**
   * Set data in data set.
   * @param data data to set
   * @param type type of data to set
   */
  void setData(void *data, ValueType type);

 private:
  std::string fieldName;
  gsl::index index;
};

}  // namespace DataSets

#endif  // DATASETS_FIELDS_HEADERS_BASEFIELD_H_
