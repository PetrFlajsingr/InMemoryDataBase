//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef DATASETS_HEADERS_MEMORYDATASET_H_
#define DATASETS_HEADERS_MEMORYDATASET_H_

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <thread>

#include <BaseDataSet.h>
#include "Types.h"
#include "FilterStructures.h"
#include "Exceptions.h"
#include "IntegerField.h"
#include "StringField.h"
#include "DoubleField.h"
#include "Utilities.h"
#include "CurrencyField.h"
#include <BaseDataProvider.h>
#include <DateTimeField.h>

namespace DataSets {
class MemoryViewDataSet;
/**
 * Data set saving data in memory.
 */
class MemoryDataSet : public BaseDataSet {
 public:
  explicit MemoryDataSet(std::string_view dataSetName);
  ~MemoryDataSet() override;
  // BaseDataSet
  void open(DataProviders::BaseDataProvider &dataProvider, const std::vector<ValueType> &fieldTypes) override;
  void openEmpty(const std::vector<std::string> &fieldNames,
                 const std::vector<ValueType> &fieldTypes) override;
  void close() override;
  void first() override;
  void last() override;
  bool next() override;
  bool previous() override;
  void sort(SortOptions &options) override;

    [[nodiscard]] std::shared_ptr<ViewDataSet> filter(const FilterOptions &options) override;

    [[nodiscard]] BaseField *fieldByName(std::string_view name) const override;

    [[nodiscard]] BaseField *fieldByIndex(gsl::index index) const override;
  std::vector<BaseField *> getFields() const override;

    [[nodiscard]] bool isFirst() const override;

    [[nodiscard]] bool isLast() const override;

    [[nodiscard]] std::vector<std::string> getFieldNames() const override;
  void append() override;
  void append(DataProviders::BaseDataProvider &dataProvider) override;

    [[nodiscard]] bool isBegin() const override;

    [[nodiscard]] bool isEnd() const override;

    [[nodiscard]] gsl::index getCurrentRecord() const override;
  void resetBegin() override;
  void resetEnd() override;
  //\ BaseDataSet
  /**
   * Create a view to all records.
   * @return view pointing to all records.
   */
  [[nodiscard]] std::shared_ptr<MemoryViewDataSet> fullView();
  /**
   * Random access iterator for iteration on raw data saved in memory.
   */
  class iterator : public std::iterator<std::random_access_iterator_tag, int> {
   public:
    iterator() = default;
    iterator(gsl::not_null<MemoryDataSet *> dataSet, gsl::index row) : dataSet(dataSet), currentRecord(row) {}
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

    DataSetRow *operator*() {
      return dataSet->data[currentRecord];
    }

    DataSetRow *operator*(int) {
      return dataSet->data[currentRecord];
    }

    DataSetRow *operator->() {
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

    DataSetRow *operator[](const int index) {
      return dataSet->data[index];
    }
   private:
    MemoryDataSet *dataSet;

    gsl::index currentRecord;
  };

  iterator begin();
  iterator end();

 protected:
    void setData(void *newData, gsl::index index, ValueType type) override;

 private:
  bool isOpen = false;

  gsl::index currentRecord = 0;  //< Pocitadlo zaznamu

  std::vector<DataSetRow *> data;

  inline gsl::index getFirst() const;
  inline gsl::index getLast() const;
  /**
   * Load all data from provider to memory.
   * @param dataProvider provider to read data from
   */
  void loadData(DataProviders::BaseDataProvider &dataProvider);
  /**
   * Add a single record from provider.
   */
  void addRecord(DataProviders::BaseDataProvider &dataProvider);
  /**
   * Create fields with provided names and types.
   * @param columns names of columns/fields
   * @param types types of fields
   */
  void createFields(std::vector<std::string> columns, std::vector<ValueType> types);
  /**
   * Set values of current row to fields.
   * @param index Index Field
   * @param searchForward Smer vyhledavani validniho zaznamu
   * @return
   */
  void setFieldValues();

  inline DataContainer &getCell(gsl::index row, gsl::index column);
};
}  // namespace DataSets

#endif //  DATASETS_HEADERS_MEMORYDATASET_H_
