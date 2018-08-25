//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef CSV_READER_INMEMORYDATASET_H
#define CSV_READER_INMEMORYDATASET_H


#include "../Interfaces/IDataset.h"

class InMemorydataset : IDataset {
private:
    enum SortOrder {ASCENDING, DESCENDING};

    struct SearchOptions {
        std::vector<std::string>    fieldNames;
        std::vector<std::string>    searchString;
    };

    struct SortOptions {
        std::vector<std::string>    fieldNames;
        std::vector<SortOrder>      order;
    };

public:

    void loadData() override = 0;

    void first() override = 0;

    void last() override = 0;

    void next() override = 0;

    void previous() override = 0;

    Field fieldByName(const std::string& name) override = 0;

};


#endif //CSV_READER_INMEMORYDATASET_H
