//
// Created by Petr Flajsingr on 2019-02-04.
//

#include <catch.hpp>
#include <Async_MemoryDataSet.h>

class MockAsyncMemoryDatasetObserver
    : public DataSets::IAsyncMemoryDataSetObserver {
 public:
  void onBeforeOpen(DataSets::Async_MemoryDataSet *sender) override {
    beforeOpenCalled = true;
  }
  void onAfterOpen(DataSets::Async_MemoryDataSet *sender) override {
    afterOpenCalled = true;
  }
  void onBeforeSort(DataSets::Async_MemoryDataSet *sender) override {
    beforeSortCalled = true;
  }
  void onAfterSort(DataSets::Async_MemoryDataSet *sender) override {
    afterSortnCalled = true;
  }
  void onBeforeFilter(DataSets::Async_MemoryDataSet *sender) override {
    beforeFilterCalled = true;
  }
  void onAfterFilter(DataSets::Async_MemoryDataSet *sender) override {
    afterFilterCalled = true;
  }
  void onBeforeAppendDataProvider(DataSets::Async_MemoryDataSet *sender) override {
    beforeAppendProviderCalled = true;
  }
  void onAfterAppendDataProvider(DataSets::Async_MemoryDataSet *sender) override {
    afterAppendProviderCalled = true;
  }

  //

  bool beforeOpenCalled = false;
  bool afterOpenCalled = false;
  bool beforeSortCalled = false;
  bool afterSortnCalled = false;
  bool beforeFilterCalled = false;
  bool afterFilterCalled = false;
  bool beforeAppendProviderCalled = false;
  bool afterAppendProviderCalled = false;
};