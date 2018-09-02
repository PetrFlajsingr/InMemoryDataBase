//
// Created by Petr Flajsingr on 02/09/2018.
//

#include "AsyncMemoryDataset.h"
#include "../Misc/Exceptions.h"
#include <future>

void AsyncMemoryDataset::open() {
    notify(BEFORE_OPEN);

    auto handle = std::async(std::launch::async, &AsyncMemoryDataset::innerOpen, this);

}

void AsyncMemoryDataset::innerOpen() {
    Memorydataset::open();
    notify(AFTER_OPEN);
}

void AsyncMemoryDataset::notify(AsyncMemoryDataset::EventType type) {
    switch(type) {
        case BEFORE_OPEN:
            for(auto observer : observers) {
                observer->onBeforeOpen(this);
            }
            break;
        case AFTER_OPEN:
            for(auto observer : observers) {
                observer->onAfterOpen(this);
            }
            break;
        case BEFORE_SORT:
            for(auto observer : observers) {
                observer->onBeforeSort(this);
            }
            break;
        case AFTER_SORT:
            for(auto observer : observers) {
                observer->onAfterSort(this);
            }
            break;
        case BEFORE_FILTER:
            for(auto observer : observers) {
                observer->onBeforeFilter(this);
            }
            break;
        case AFTER_FILTER:
            for(auto observer : observers) {
                observer->onAfterFilter(this);
            }
            break;
        case BEFORE_APPEND:
            for(auto observer : observers) {
                observer->onBeforeAppendDataProvider(this);
            }
            break;
        case AFTER_APPEND:
            for(auto observer : observers) {
                observer->onAfterAppendDataProvider(this);
            }
            break;
    }
}

void AsyncMemoryDataset::addObserver(IAsyncMemoryDatasetObserver *observer) {
    observers.push_back(observer);
}

void AsyncMemoryDataset::removeObserver(IAsyncMemoryDatasetObserver *observer) {
    observers.erase(std::find(observers.begin(), observers.end(), observer));
}

void AsyncMemoryDataset::innerFilter(FilterOptions &options) {
    throw NotImplementedException();
}

void AsyncMemoryDataset::innerSort(unsigned long fieldIndex, SortOrder sortOrder) {
    throw NotImplementedException();
}

void AsyncMemoryDataset::innerAppendDataProvider() {
    throw NotImplementedException();
}

void AsyncMemoryDataset::filter(FilterOptions &options) {
    throw NotImplementedException();
}

void AsyncMemoryDataset::sort(unsigned long fieldIndex, SortOrder sortOrder) {
    throw NotImplementedException();
}

void AsyncMemoryDataset::appendDataProvider(IDataProvider *provider) {
    throw NotImplementedException();
}
