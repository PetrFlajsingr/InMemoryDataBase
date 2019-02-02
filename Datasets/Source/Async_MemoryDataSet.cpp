//
// Created by Petr Flajsingr on 02/09/2018.
//

#include "Async_MemoryDataSet.h"
#include <future>

void DataSets::Async_MemoryDataSet::open() {
  notify(EventType::BeforeOpen);
  auto handle = std::async(std::launch::async,
                           &Async_MemoryDataSet::innerOpen,
                           this);
}

void DataSets::Async_MemoryDataSet::innerOpen() {
  MemoryDataSet::open();
  notify(EventType::AfterOpen);
}

void DataSets::Async_MemoryDataSet::notify(Async_MemoryDataSet::EventType type) {
  switch (type) {
    case EventType::BeforeOpen:
      for (auto observer : observers) {
        observer->onBeforeOpen(this);
      }
      break;
    case EventType::AfterOpen:
      for (auto observer : observers) {
        observer->onAfterOpen(this);
      }
      break;
    case EventType::BeforeSort:
      for (auto observer : observers) {
        observer->onBeforeSort(this);
      }
      break;
    case EventType::AfterSort:
      for (auto observer : observers) {
        observer->onAfterSort(this);
      }
      break;
    case EventType::BeforeFilter:
      for (auto observer : observers) {
        observer->onBeforeFilter(this);
      }
      break;
    case EventType::AfterFilter:
      for (auto observer : observers) {
        observer->onAfterFilter(this);
      }
      break;
    case EventType::BeforeAppend:
      for (auto observer : observers) {
        observer->onBeforeAppendDataProvider(this);
      }
      break;
    case EventType::AfterAppend:
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
  notify(EventType::BeforeFilter);
  auto handle = std::async(std::launch::async,
                           &Async_MemoryDataSet::innerFilter,
                           this,
                           std::ref(options));
}

void DataSets::Async_MemoryDataSet::innerFilter(const FilterOptions &options) {
  MemoryDataSet::filter(options);
  notify(EventType::AfterFilter);
}

void DataSets::Async_MemoryDataSet::sort(SortOptions &options) {
  notify(EventType::BeforeSort);
  auto handle = std::async(std::launch::async,
                           &Async_MemoryDataSet::innerSort,
                           this,
                           std::ref(options));
}

void DataSets::Async_MemoryDataSet::innerSort(SortOptions &options) {
  MemoryDataSet::sort(options);
  notify(EventType::AfterSort);
}

void DataSets::Async_MemoryDataSet::appendDataProvider(DataProviders::BaseDataProvider *provider) {
  notify(EventType::BeforeAppend);
  auto handle = std::async(std::launch::async,
                           &Async_MemoryDataSet::innerAppendDataProvider,
                           this,
                           provider);
}

void DataSets::Async_MemoryDataSet::innerAppendDataProvider(DataProviders::BaseDataProvider *provider) {
  MemoryDataSet::appendDataProvider(provider);
  notify(EventType::AfterAppend);
}

DataSets::Async_MemoryDataSet::Async_MemoryDataSet(std::string_view dataSetName)
    : MemoryDataSet(dataSetName) {}
