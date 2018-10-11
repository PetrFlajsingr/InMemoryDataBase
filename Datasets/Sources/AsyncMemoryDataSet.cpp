//
// Created by Petr Flajsingr on 02/09/2018.
//

#include "AsyncMemoryDataSet.h"
#include "Exceptions.h"
#include <future>

void AsyncMemoryDataSet::open() {
    notify(BEFORE_OPEN);
    auto handle = std::async(std::launch::async,
            &AsyncMemoryDataSet::innerOpen,
            this);
}

void AsyncMemoryDataSet::innerOpen() {
    MemoryDataSet::open();
    notify(AFTER_OPEN);
}

void AsyncMemoryDataSet::notify(AsyncMemoryDataSet::EventType type) {
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

void AsyncMemoryDataSet::addObserver(IAsyncMemoryDatasetObserver *observer) {
    observers.push_back(observer);
}

void AsyncMemoryDataSet::removeObserver(IAsyncMemoryDatasetObserver *observer) {
    observers.erase(std::find(observers.begin(), observers.end(), observer));
}

void AsyncMemoryDataSet::filter(const FilterOptions &options) {
    notify(BEFORE_FILTER);
    auto handle = std::async(std::launch::async,
            &AsyncMemoryDataSet::innerFilter,
            this,
            std::ref(options));
}

void AsyncMemoryDataSet::innerFilter(const FilterOptions &options) {
    MemoryDataSet::filter(options);
    notify(AFTER_FILTER);
}

void AsyncMemoryDataSet::sort(uint64_t fieldIndex, SortOrder sortOrder) {
    notify(BEFORE_SORT);
    auto handle = std::async(std::launch::async,
            &AsyncMemoryDataSet::innerSort,
            this,
            fieldIndex,
            sortOrder);
}

void AsyncMemoryDataSet::innerSort(uint64_t fieldIndex, SortOrder sortOrder) {
    MemoryDataSet::sort(fieldIndex, sortOrder);
    notify(AFTER_SORT);
}

void AsyncMemoryDataSet::appendDataProvider(IDataProvider *provider) {
    notify(BEFORE_APPEND);
    auto handle = std::async(std::launch::async,
            &AsyncMemoryDataSet::innerAppendDataProvider,
            this,
            provider);
}

void AsyncMemoryDataSet::innerAppendDataProvider(IDataProvider *provider) {
    MemoryDataSet::appendDataProvider(provider);
    notify(AFTER_APPEND);
}
