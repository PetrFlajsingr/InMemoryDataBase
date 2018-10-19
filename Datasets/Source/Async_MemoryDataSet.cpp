//
// Created by Petr Flajsingr on 02/09/2018.
//

#include "Async_MemoryDataSet.h"
#include <future>

void DataSets::Async_MemoryDataSet::open() {
  notify(BEFORE_OPEN);
  auto handle = std::async(std::launch::async,
                           &Async_MemoryDataSet::innerOpen,
                           this);
}

void DataSets::Async_MemoryDataSet::innerOpen() {
  MemoryDataSet::open();
  notify(AFTER_OPEN);
}

void DataSets::Async_MemoryDataSet::notify(Async_MemoryDataSet::EventType type) {
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

void DataSets::Async_MemoryDataSet::addObserver(IAsyncMemoryDataSetObserver *observer) {
  observers.push_back(observer);
}

void DataSets::Async_MemoryDataSet::removeObserver(IAsyncMemoryDataSetObserver *observer) {
  observers.erase(std::find(observers.begin(), observers.end(), observer));
}

void DataSets::Async_MemoryDataSet::filter(const FilterOptions &options) {
  notify(BEFORE_FILTER);
  auto handle = std::async(std::launch::async,
                           &Async_MemoryDataSet::innerFilter,
                           this,
                           std::ref(options));
}

void DataSets::Async_MemoryDataSet::innerFilter(const FilterOptions &options) {
  MemoryDataSet::filter(options);
  notify(AFTER_FILTER);
}

void DataSets::Async_MemoryDataSet::sort(uint64_t fieldIndex,
    SortOrder sortOrder) {
  notify(BEFORE_SORT);
  auto handle = std::async(std::launch::async,
                           &Async_MemoryDataSet::innerSort,
                           this,
                           fieldIndex,
                           sortOrder);
}

void DataSets::Async_MemoryDataSet::innerSort(uint64_t fieldIndex,
    SortOrder sortOrder) {
  MemoryDataSet::sort(fieldIndex, sortOrder);
  notify(AFTER_SORT);
}

void DataSets::Async_MemoryDataSet::appendDataProvider(DataProviders::BaseDataProvider *provider) {
  notify(BEFORE_APPEND);
  auto handle = std::async(std::launch::async,
                           &Async_MemoryDataSet::innerAppendDataProvider,
                           this,
                           provider);
}

void DataSets::Async_MemoryDataSet::innerAppendDataProvider(DataProviders::BaseDataProvider *provider) {
  MemoryDataSet::appendDataProvider(provider);
  notify(AFTER_APPEND);
}
