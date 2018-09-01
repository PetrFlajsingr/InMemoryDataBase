//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef CSV_READER_FILTERSTRUCTURES_H
#define CSV_READER_FILTERSTRUCTURES_H

#include <vector>
#include <string>

enum FilterOption {
    EQUALS,
    STARTS_WITH,
    CONTAINS,
    ENDS_WITH,
    NOT_CONTAINS,
    NOT_STARTS_WITH,
    NOT_ENDS_WITH};

struct FilterItem {
    unsigned long fieldIndex;
    std::string searchString;
    FilterOption filterOption;
};

struct FilterOptions {
    std::vector<FilterItem> options;

    void addOption(const unsigned long fieldIndex,
                   const std::string &searchString,
                   const FilterOption filterOption){
        FilterItem item{
                fieldIndex,
                searchString,
                filterOption
        };

        options.push_back(item);
    }
};
#endif //CSV_READER_FILTERSTRUCTURES_H
