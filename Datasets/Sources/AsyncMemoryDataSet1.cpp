//
// Created by Petr Flajsingr on 02/09/2018.
//

#include "AsyncMemoryDataSet1.h"
#include <future>

void DataSets::AsyncMemoryDataSet1::open() {
  notify(BEFORE_OPEN);
  auto handle = std::async(std::launch::async,
                           &AsyncMemoryDataSet1::innerOpen,
                           this);
}

void DataSets::AsyncMemoryDataSet1::innerOpen() {
  MemoryDataSet::open();
  notify(AFTER_OPEN);
}

void DataSets::AsyncMemoryDataSet1::notify(AsyncMemoryDataSet1::EventType type) {
  switch (type) {
    case BEFORE_OPEN:
      for (auto observer : observers) {
        observer->onBeforeOpen(this);
      }
      break;
    case AFTER_OPEN:
      for (auto observer : observers) {
        observer->onAfterOpen(this);
      }
      break;
    case BEFORE_SORT:
      for (auto observer : observers) {
        observer->onBeforeSort(this);
      }
      break;
    case AFTER_SORT:
      for (auto observer : observers) {
        observer->onAfterSort(this);
      }
      break;
    case BEFORE_FILTER:
      for (auto observer : observers) {
        observer->onBeforeFilter(this);
      }
      break;
    case AFTER_FILTER:
      for (auto observer : observers) {
        observer->onAfterFilter(this);
      }
      break;
    case BEFORE_APPEND:
      for (auto observer : observers) {
        observer->onBeforeAppendDataProvider(this);
      }
      break;
    case AFTER_APPEND:
      for (auto observer : observers) {
        observer->onAfterAppendDataProvider(this);
      }
      break;
  }
}

void DataSets::AsyncMemoryDataSet1::addObserver(IAsyncMemoryDataSetObserver *observer) {
  observers.push_back(observer);
}

void DataSets::AsyncMemoryDataSet1::removeObserver(IAsyncMemoryDataSetObserver *observer) {
  observers.erase(std::find(observers.begin(), observers.end(), observer));
}

void DataSets::AsyncMemoryDataSet1::filter(const FilterOptions &options) {
  notify(BEFORE_FILTER);
  auto handle = std::async(std::launch::async,
                           &AsyncMemoryDataSet1::innerFilter,
                           this,
                           std::ref(options));
}

void DataSets::AsyncMemoryDataSet1::innerFilter(const FilterOptions &options) {
  MemoryDataSet::filter(options);
  notify(AFTER_FILTER);
}

void DataSets::AsyncMemoryDataSet1::sort(uint64_t fieldIndex,
    SortOrder sortOrder) {
  notify(BEFORE_SORT);
  auto handle = std::async(std::launch::async,
                           &AsyncMemoryDataSet1::innerSort,
                           this,
                           fieldIndex,
                           sortOrder);
}

void DataSets::AsyncMemoryDataSet1::innerSort(uint64_t fieldIndex,
    SortOrder sortOrder) {
  MemoryDataSet::sort(fieldIndex, sortOrder);
  notify(AFTER_SORT);
}

void DataSets::AsyncMemoryDataSet1::appendDataProvider(IDataProvider *provider) {
  notify(BEFORE_APPEND);
  auto handle = std::async(std::launch::async,
                           &AsyncMemoryDataSet1::innerAppendDataProvider,
                           this,
                           provider);
}

void DataSets::AsyncMemoryDataSet1::innerAppendDataProvider(IDataProvider *provider) {
  MemoryDataSet::appendDataProvider(provider);
  notify(AFTER_APPEND);
}
