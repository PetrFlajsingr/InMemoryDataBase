//
// Created by Petr Flajsingr on 2019-02-08.
//

#ifndef PROJECT_MEMORYVIEWDATASET_H
#define PROJECT_MEMORYVIEWDATASET_H

#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include <ViewDataSet.h>
#include <FieldFactory.h>

namespace DataSets {

class MemoryViewDataSet : public ViewDataSet {
 public:
  MemoryViewDataSet(std::string_view dataSetName,
                    const std::vector<std::string> &fieldNames,
                    const std::vector<ValueType> &fieldTypes,
                    const std::vector<std::pair<int,
                                                int>> &fieldIndices);

  void open(DataProviders::BaseDataProvider &dataProvider,
            const std::vector<ValueType> &fieldTypes) override;
  void openEmpty(const std::vector<std::string> &fieldNames,
                 const std::vector<ValueType> &fieldTypes) override;
  void close() override;
  void first() override;
  void last() override;
  bool next() override;
  bool previous() override;
  bool isFirst() const override;
  bool isBegin() const override;
  bool isLast() const override;
  bool isEnd() const override;
  BaseField *fieldByName(std::string_view name) const override;
  BaseField *fieldByIndex(gsl::index index) const override;
  std::vector<BaseField *> getFields() const override;
  std::vector<std::string> getFieldNames() const override;
  gsl::index getCurrentRecord() const override;
  void append() override;
  void append(DataProviders::BaseDataProvider &dataProvider) override;
  void sort(SortOptions &options) override;
  std::shared_ptr<ViewDataSet> filter(const FilterOptions &options) override;
  bool findFirst(FilterItem &item) override;
  void resetBegin() override;
  void resetEnd() override;
  void setData(void *data, gsl::index index, ValueType type) override;

  std::vector<std::vector<DataSetRow *>> *rawData();

  class iterator : public std::iterator<std::random_access_iterator_tag, int> {
   public:
    iterator() = default;

    iterator(gsl::not_null<MemoryViewDataSet *> dataSet, gsl::index row)
        : dataSet(dataSet), currentRecord(row) {}

    iterator(const iterator &other) : dataSet(other.dataSet),
                                      currentRecord(other.currentRecord) {}

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

    const iterator operator++(int) {
      iterator result = *this;
      ++(*this);
      return result;
    }

    bool operator==(const iterator &rhs) const {
      return currentRecord == rhs.currentRecord;
    }

    bool operator!=(const iterator &other) const {
      return !(*this == other);
    }

    std::vector<DataSetRow *> &operator*() {
      return dataSet->data[currentRecord];
    }

    std::vector<DataSetRow *> &operator*(int) {
      return dataSet->data[currentRecord];
    }

    std::vector<DataSetRow *> &operator->() {
      return dataSet->data[currentRecord];
    }

    iterator &operator--() {
      currentRecord--;
      return *this;
    }

    const iterator operator--(int) {
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

    bool operator<(const iterator &rhs) {
      return currentRecord < rhs.currentRecord;
    }

    bool operator>(const iterator &rhs) {
      return currentRecord > rhs.currentRecord;
    }

    bool operator<=(const iterator &rhs) {
      return currentRecord <= rhs.currentRecord;
    }

    bool operator>=(const iterator &rhs) {
      return currentRecord >= rhs.currentRecord;
    }

    iterator &operator+=(const int rhs) {
      currentRecord += rhs;
      return *this;
    }

    iterator &operator-=(const int rhs) {
      currentRecord -= rhs;
      return *this;
    }

    std::vector<DataSetRow *> &operator[](const int index) {
      return dataSet->data[index];
    }

   private:
    MemoryViewDataSet *dataSet;

    gsl::index currentRecord;
  };

  iterator begin();

  iterator end();

  DataSetRow *getNullRow(gsl::index tableIndex);

  const gsl::index maskTableShift = 16;
  const gsl::index maskTableIndex = 0xFF0000;
  const gsl::index maskColumnIndex = 0x00FFFF;

 private:
  std::vector<std::vector<DataSetRow *>> data;

  std::vector<DataSetRow *> nullRecords;

  gsl::index currentRecord = 0;

  void setFieldValues(gsl::index index);

  void createFields(const std::vector<std::string> &columns,
                    const std::vector<ValueType> &types,
                    const std::vector<std::pair<int,
                                                int>> &fieldIndices);

  void createNullRows(const std::vector<std::pair<int,
                                                  int>> &fieldIndices,
                      const std::vector<ValueType> &fieldTypes);

  friend class MemoryDataSet;
};
}  // namespace DataSets

#endif //PROJECT_MEMORYVIEWDATASET_H
