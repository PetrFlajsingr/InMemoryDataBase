//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef CSV_READER_DATAACCESSOR_H
#define CSV_READER_DATAACCESSOR_H


#include <string>
#include <vector>
#include "IDataProvider.h"

enum SortOrder {ASCENDING, DESCENDING};

class DataAccessor {
private:

public:
    DataAccessor();

    ~DataAccessor();

    void sort(SortOptions& options);

    std::vector<std::vector<std::string>> find(SearchOptions& options);

    struct SearchOptions {
        std::vector<std::string>    columnNames;
        std::vector<std::string>    searchString;
    };

    struct SortOptions {
        std::vector<std::string>    columnNames;
        std::vector<SortOrder>      order;
    };

};


#endif //CSV_READER_DATAACCESSOR_H
