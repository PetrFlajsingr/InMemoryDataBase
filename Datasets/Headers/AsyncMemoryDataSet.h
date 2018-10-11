//
// Created by Petr Flajsingr on 02/09/2018.
//

#ifndef DATASETS_HEADERS_ASYNCMEMORYDATASET_H_
#define DATASETS_HEADERS_ASYNCMEMORYDATASET_H_

#include "MemoryDataSet.h"
#include <vector>
#include <algorithm>

class IAsyncMemoryDatasetObserver;

class AsyncMemoryDataSet : public MemoryDataSet {
 protected:
    std::vector<IAsyncMemoryDatasetObserver*> observers;

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

    void appendDataProvider(IDataProvider* provider) override;

    void addObserver(IAsyncMemoryDatasetObserver* observer);

    void removeObserver(IAsyncMemoryDatasetObserver* observer);
};

class IAsyncMemoryDatasetObserver {
 public:
    virtual void onBeforeOpen(AsyncMemoryDataSet* sender)= 0;
    virtual void onAfterOpen(AsyncMemoryDataSet* sender)= 0;

    virtual void onBeforeSort(AsyncMemoryDataSet* sender)= 0;
    virtual void onAfterSort(AsyncMemoryDataSet* sender)= 0;

    virtual void onBeforeFilter(AsyncMemoryDataSet* sender)= 0;
    virtual void onAfterFilter(AsyncMemoryDataSet* sender)= 0;

    virtual void onBeforeAppendDataProvider(AsyncMemoryDataSet* sender)= 0;
    virtual void onAfterAppendDataProvider(AsyncMemoryDataSet* sender)= 0;

    virtual ~IAsyncMemoryDatasetObserver() = default;
};

#endif //  DATASETS_HEADERS_ASYNCMEMORYDATASET_H_
