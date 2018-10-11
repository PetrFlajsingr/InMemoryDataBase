//
// Created by Petr Flajsingr on 02/09/2018.
//

#ifndef DATASETS_HEADERS_ASYNCMEMORYDATASET_H_
#define DATASETS_HEADERS_ASYNCMEMORYDATASET_H_

#include "MemoryDataSet.h"
#include <vector>
#include <algorithm>

namespace DataSets {
class IAsyncMemoryDataSetObserver;

class Async_MemoryDataSet : public MemoryDataSet {
 protected:
  std::vector<IAsyncMemoryDataSetObserver *> observers;

  enum EventType {
    BEFORE_OPEN,
    AFTER_OPEN,
    BEFORE_SORT,
    AFTER_SORT,
    BEFORE_FILTER,
    AFTER_FILTER,
    BEFORE_APPEND,
    AFTER_APPEND
  };

  void innerOpen();

  void innerFilter(const FilterOptions &options);

  void innerSort(uint64_t fieldIndex, SortOrder sortOrder);

  void innerAppendDataProvider(IDataProvider *provider);

  void notify(EventType type);

 public:
  void open() override;

  void filter(const FilterOptions &options) override;

  void sort(uint64_t fieldIndex, SortOrder sortOrder) override;

  void appendDataProvider(IDataProvider *provider) override;

  void addObserver(IAsyncMemoryDataSetObserver *observer);

  void removeObserver(IAsyncMemoryDataSetObserver *observer);
};

class IAsyncMemoryDataSetObserver {
 public:
  virtual void onBeforeOpen(Async_MemoryDataSet *sender) = 0;

  virtual void onAfterOpen(Async_MemoryDataSet *sender) = 0;

  virtual void onBeforeSort(Async_MemoryDataSet *sender) = 0;

  virtual void onAfterSort(Async_MemoryDataSet *sender) = 0;

  virtual void onBeforeFilter(Async_MemoryDataSet *sender) = 0;

  virtual void onAfterFilter(Async_MemoryDataSet *sender) = 0;

  virtual void onBeforeAppendDataProvider(Async_MemoryDataSet *sender) = 0;

  virtual void onAfterAppendDataProvider(Async_MemoryDataSet *sender) = 0;

  virtual ~IAsyncMemoryDataSetObserver() = default;
};
}

#endif //  DATASETS_HEADERS_ASYNCMEMORYDATASET_H_
