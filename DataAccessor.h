//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef CSV_READER_DATAACCESSOR_H
#define CSV_READER_DATAACCESSOR_H


#include <string>
#include <vector>

enum SortOrder {ASCENDING, DESCENDING};

class DataAccessor {
private:

public:

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
