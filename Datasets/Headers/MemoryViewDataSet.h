//
// Created by Petr Flajsingr on 2019-02-08.
//

#ifndef PROJECT_MEMORYVIEWDATASET_H
#define PROJECT_MEMORYVIEWDATASET_H

#include <vector>
#include <string>
#include <algorithm>
#include <MemoryDataSet.h>
#include <FieldFactory.h>

namespace DataSets {

class MemoryViewDataSet : BaseDataSet {
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
  void filter(const FilterOptions &options) override;
  bool findFirst(FilterItem &item) override;
  void resetBegin() override;
  void resetEnd() override;
  void setData(void *data, gsl::index index, ValueType type) override;

 private:
  std::vector<std::vector<DataSetRow *>> data;

  const std::vector<DataSetRow *> stopItem{};

  gsl::index currentRecord = 0;

  void setFieldValues(gsl::index index);

  const gsl::index maskTableIndex = 0xFF0000;
  const gsl::index maskColumnIndex = 0x00FFFF;

  void createFields(const std::vector<std::string> &columns,
                    const std::vector<ValueType> &types,
                    const std::vector<std::pair<int,
                                                int>> &fieldIndices);
};
}  // namespace DataSets

#endif //PROJECT_MEMORYVIEWDATASET_H
