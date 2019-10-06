//
// Created by Petr Flajsingr on 2019-02-08.
//

#ifndef PROJECT_MEMORYVIEWDATASET_H
#define PROJECT_MEMORYVIEWDATASET_H

#include <FieldFactory.h>
#include <ViewDataSet.h>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace DataBase {
class MemoryDataBase;
}

namespace DataSets {
class MemoryDataSet;
/**
 * View to MemoryDataSet.
 * May point to multiple data sets at the same time.
 */
class MemoryViewDataSet : public ViewDataSet {
public:
  /**
   *
   * @param dataSetName name of the view
   * @param fieldNames names of fields
   * @param fieldTypes types of values stored in columns
   * @param fieldIndices indices of fields see @code BaseField @endcode for explanation @see BaseField
   */
  MemoryViewDataSet(std::string_view dataSetName, const std::vector<std::string> &fieldNames,
                    const std::vector<ValueType> &fieldTypes, const std::vector<std::pair<int, int>> &fieldIndices);
  // BaseDataSet
  void open(DataProviders::BaseDataProvider &dataProvider, const std::vector<ValueType> &fieldTypes) override;
  void openEmpty(const std::vector<std::string> &fieldNames, const std::vector<ValueType> &fieldTypes) override;
  void close() override;
  void first() override;
  void last() override;
  bool next() override;
  bool previous() override;

  [[nodiscard]] bool isFirst() const override;

  [[nodiscard]] bool isBegin() const override;

  [[nodiscard]] bool isLast() const override;

  [[nodiscard]] bool isEnd() const override;

  [[nodiscard]] BaseField *fieldByName(std::string_view name) const override;

  [[nodiscard]] BaseField *fieldByIndex(gsl::index index) const override;

  [[nodiscard]] std::vector<BaseField *> getFields() const override;

  [[nodiscard]] std::vector<std::string> getFieldNames() const override;

  [[nodiscard]] gsl::index getCurrentRecord() const override;
  void append() override;
  void append(DataProviders::BaseDataProvider &dataProvider) override;
  void sort(SortOptions &options) override;
  std::shared_ptr<ViewDataSet> filter(const FilterOptions &options) override;
  void resetBegin() override;
  void resetEnd() override;
  void setData(void *data, gsl::index index, ValueType type) override;
  //\ BaseDataSet
  /**
   * Get inner data representation
   * @return inner data
   */
  std::vector<std::vector<DataSetRow *>> *rawData();
  /**
   *
   * @return number of data sets this view points to
   */
  gsl::index getTableCount();
  /**
   * Add shared_ptr to parent to not lose data this view points to
   */
  void addParent(std::shared_ptr<MemoryDataSet> &parent);
  /**
   * Add shared_ptr to parent to not lose data this view points to
   */
  void addParents(const std::vector<std::shared_ptr<MemoryDataSet>> &parents);
  /**
   * @return data sets this view points to
   */
  [[nodiscard]] const std::vector<std::shared_ptr<MemoryDataSet>> &getParents() const;
  /**
   * Create a data set from data this view points to.
   * @return materialised view
   */
  std::shared_ptr<MemoryDataSet> toDataSet();
  /**
   * Iterate over raw data.
   */
  class iterator : public std::iterator<std::random_access_iterator_tag, int> {
  public:
    iterator() = default;

    iterator(gsl::not_null<MemoryViewDataSet *> dataSet, gsl::index row) : dataSet(dataSet), currentRecord(row) {}

    iterator(const iterator &other) : dataSet(other.dataSet), currentRecord(other.currentRecord) {}

    iterator &operator=(const iterator &other) {
      if (this != &other) {
        dataSet = other.dataSet;
        currentRecord = other.currentRecord;
      }
      return *this;
    }

    iterator &operator=(iterator &&other) noexcept {
      if (this != &other) {
        dataSet = other.dataSet;
        currentRecord = other.currentRecord;
      }
      return *this;
    }

    iterator &operator++() {
      currentRecord++;
      return *this;
    }

    iterator operator++(int) {
      iterator result = *this;
      ++(*this);
      return result;
    }

    bool operator==(const iterator &rhs) const { return currentRecord == rhs.currentRecord; }

    bool operator!=(const iterator &other) const { return !(*this == other); }

    std::vector<DataSetRow *> &operator*() { return dataSet->data[currentRecord]; }

    std::vector<DataSetRow *> &operator*(int) { return dataSet->data[currentRecord]; }

    std::vector<DataSetRow *> &operator->() { return dataSet->data[currentRecord]; }

    iterator &operator--() {
      currentRecord--;
      return *this;
    }

    iterator operator--(int) {
      iterator result = *this;
      --(*this);
      return result;
    }

    iterator operator+(const int rhs) {
      iterator res = *this;
      res.currentRecord += rhs;
      return res;
    }

    iterator operator-(const int rhs) {
      iterator res = *this;
      res.currentRecord -= rhs;
      return res;
    }

    bool operator<(const iterator &rhs) { return currentRecord < rhs.currentRecord; }

    bool operator>(const iterator &rhs) { return currentRecord > rhs.currentRecord; }

    bool operator<=(const iterator &rhs) { return currentRecord <= rhs.currentRecord; }

    bool operator>=(const iterator &rhs) { return currentRecord >= rhs.currentRecord; }

    iterator &operator+=(const int rhs) {
      currentRecord += rhs;
      return *this;
    }

    iterator &operator-=(const int rhs) {
      currentRecord -= rhs;
      return *this;
    }

    std::vector<DataSetRow *> &operator[](const int index) { return dataSet->data[index]; }

  private:
    MemoryViewDataSet *dataSet;

    gsl::index currentRecord;
  };

  iterator begin();
  iterator end();
  /**
   * Null row for empty table records.
   * @param tableIndex
   * @return
   */
  DataSetRow *getNullRow(gsl::index tableIndex);
  /**
   * Set fields to limit view to data.
   * @param fieldNames names of fields to allow
   */
  void setAllowedFields(const std::vector<std::string> &fieldNames);

private:
  std::vector<BaseField *> allowedFields;

  std::vector<std::vector<DataSetRow *>> data;

  std::vector<DataSetRow *> nullRecords;

  gsl::index currentRecord = 0;

  void setFieldValues(gsl::index index);

  void createFields(const std::vector<std::string> &columns, const std::vector<ValueType> &types,
                    const std::vector<std::pair<int, int>> &fieldIndices);

  void createNullRows(const std::vector<std::pair<int, int>> &fieldIndices, const std::vector<ValueType> &fieldTypes);

  friend class MemoryDataSet;
  std::vector<std::shared_ptr<MemoryDataSet>> parents;
};
} // namespace DataSets

#endif // PROJECT_MEMORYVIEWDATASET_H
