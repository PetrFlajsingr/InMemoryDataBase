//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef DATASETS_HEADERS_BASEDATASET_H_
#define DATASETS_HEADERS_BASEDATASET_H_

#include <string>
#include <vector>
#include <cstring>
#include "BaseDataProvider.h"
#include "Types.h"
#include "FilterStructures.h"
#include "SortStructures.h"

namespace DataSets {
class BaseField;
class ViewDataSet;
/**
 * Base class for data sets.
 */
class BaseDataSet {
 public:
  explicit BaseDataSet(std::string_view dataSetName) : dataSetName(dataSetName) {};
  /**
   * Load data from data provider. Names of fields (columns) is loaded from provider header.
   * @param dataProvider source of data
   * @param fieldTypes type of data to store
   */
  virtual void open(DataProviders::BaseDataProvider &dataProvider,
                    const std::vector<ValueType> &fieldTypes) = 0;
  /**
   * Open data set without filling it with data.
   * @param fieldNames names of fields
   * @param fieldTypes types of data to store
   */
  virtual void openEmpty(const std::vector<std::string> &fieldNames,
                         const std::vector<ValueType> &fieldTypes) = 0;
  /**
   * Close data set making it unable to provide data.
   */
  virtual void close() = 0;
  /**
   * Move to first data set record.
   */
  virtual void first() = 0;
  /**
   * Move to last data set record.
   */
  virtual void last() = 0;
  /**
   * Move to next data set record.
   * @return true if the move was successfull (there is more data), false otherwise
   */
  virtual bool next() = 0;
  /**
   * Move to previous data set record.
   * @return true if the move was successfull (there is more data), false otherwise
   */
  virtual bool previous() = 0;
  /**
   *
   * @return true if the current record is the first one, false otherwise
   */
  virtual bool isFirst() const = 0;
  /**
   *
   * @return true if the first record can be accessed by calling next(), false otherwise
   */
  virtual bool isBegin() const = 0;
  /**
   *
   * @return true if the current record is the last one, false otherwise
   */
  virtual bool isLast() const = 0;
  /**
   *
   * @return true if the last record can be accessed by calling previous(), false otherwise
   */
  virtual bool isEnd() const = 0;
  /**
   * Find field with a provided name.
   * @param name name of the desired field
   * @return field with the desired name
   */
  virtual BaseField *fieldByName(std::string_view name) const = 0;
  /**
   * Get field by its index.
   * @param index index of a field in the data set
   * @return field with the desired index
   */
  virtual BaseField *fieldByIndex(gsl::index index) const = 0;
  /**
   * Get all fields in data set in the same order they were created.
   * @return all fields in data set
   */
  virtual std::vector<BaseField *> getFields() const = 0;
  /**
   *
   * @return names of all fields in data set
   */
  virtual std::vector<std::string> getFieldNames() const = 0;
  /**
   *
   * @return count of columns/fields in data set
   */
  gsl::index getColumnCount() const;
  /**
   * Get index of current record in data set
   * @return index of current record
   */
  virtual gsl::index getCurrentRecord() const = 0;
  /**
   * Create an empty row at the end of the data set.
   */
  virtual void append() = 0;
  /**
   * Append provider's data to the end of the data set
   * @param dataProvider
   */
  virtual void append(DataProviders::BaseDataProvider &dataProvider) = 0;
  /**
   * Sort data by multiple keys.
   * @param options
   */
  virtual void sort(SortOptions &options) = 0;
  /**
   * Filter data based on provided options.
   * @param options
   * @return View to the desired data
   */
  virtual std::shared_ptr<ViewDataSet> filter(const FilterOptions &options) = 0;
  /**
   * Reset row cursor to point just before the first record.
   */
  virtual void resetBegin() = 0;
  /**
   * Reset row cursor to point just after the first record.
   */
  virtual void resetEnd() = 0;
  /**
   * Change data set's name
   * @param name new name
   */
  void setName(const std::string &name) {
    dataSetName = name;
  }
  /**
   *
   * @return name of the data set
   */
  std::string getName() const {
    return dataSetName;
  }
  virtual ~BaseDataSet() = default;

 protected:
  friend class BaseField;  //< to access BaseField::setData(...)

  std::vector<std::shared_ptr<BaseField>> fields;

  std::string dataSetName;

  unsigned long columnCount = 0;
  /**
   * Set data in field. Should be used when next()/previous() is called.
   * @param field field to set data to
   * @param data data to set
   */
  void setFieldData(BaseField *field, void *data);
  /**
   * Set inner data set data.
   * This method is called via BaseField.
   * @param data data to set
   * @param index column index
   * @param type type of provided data
   */
  virtual void setData(void *data, gsl::index index, ValueType type) = 0;
};
}  // namespace DataSets

#endif //  DATASETS_HEADERS_BASEDATASET_H_
