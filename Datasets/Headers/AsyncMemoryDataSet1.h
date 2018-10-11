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

class AsyncMemoryDataSet1 : public MemoryDataSet {
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
  virtual void onBeforeOpen(AsyncMemoryDataSet1 *sender) = 0;

  virtual void onAfterOpen(AsyncMemoryDataSet1 *sender) = 0;

  virtual void onBeforeSort(AsyncMemoryDataSet1 *sender) = 0;

  virtual void onAfterSort(AsyncMemoryDataSet1 *sender) = 0;

  virtual void onBeforeFilter(AsyncMemoryDataSet1 *sender) = 0;

  virtual void onAfterFilter(AsyncMemoryDataSet1 *sender) = 0;

  virtual void onBeforeAppendDataProvider(AsyncMemoryDataSet1 *sender) = 0;

  virtual void onAfterAppendDataProvider(AsyncMemoryDataSet1 *sender) = 0;

  virtual ~IAsyncMemoryDataSetObserver() = default;
};
}

#endif //  DATASETS_HEADERS_ASYNCMEMORYDATASET_H_
