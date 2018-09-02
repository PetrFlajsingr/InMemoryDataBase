//
// Created by Petr Flajsingr on 02/09/2018.
//

#ifndef CSV_READER_ASYNCHRONOUSMEMORYDATASET_H
#define CSV_READER_ASYNCHRONOUSMEMORYDATASET_H

#include "Memorydataset.h"

class AsyncMemoryDataset : public Memorydataset {
protected:
    void innerOpen();

    void innerFilter(FilterOptions &options);

    void innerSort(unsigned long fieldIndex, SortOrder sortOrder);

    void innerAppendDataProvider();
public:
    void open() override;

    void filter(FilterOptions &options) override;

    void sort(unsigned long fieldIndex, SortOrder sortOrder) override;

    void appendDataProvider(IDataProvider* provider) override;
};

class AsyncMemoryDatasetObserver {
    virtual void onBeforeOpen(AsyncMemoryDataset* sender)= 0;
    virtual void onAfterOpen(AsyncMemoryDataset* sender)= 0;

    virtual void onBeforeSort(AsyncMemoryDataset* sender)= 0;
    virtual void onAfterSort(AsyncMemoryDataset* sender)= 0;

    virtual void onBeforeFilter(AsyncMemoryDataset* sender)= 0;
    virtual void onAfterFilter(AsyncMemoryDataset* sender)= 0;

    virtual void onBeforeAppendDataProvider(AsyncMemoryDataset* sender)= 0;
    virtual void onAfterAppendDataProvider(AsyncMemoryDataset* sender)= 0;
};

#endif //CSV_READER_ASYNCHRONOUSMEMORYDATASET_H
