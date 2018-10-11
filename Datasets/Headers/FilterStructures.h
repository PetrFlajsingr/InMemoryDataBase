//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_HEADERS_FILESTRUCTURES_H_
#define DATASETS_HEADERS_FILESTRUCTURES_H_

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
    uint64_t fieldIndex;
    std::string searchString;
    FilterOption filterOption;
};

struct FilterOptions {
    std::vector<FilterItem> options;

    void addOption(const uint64_t fieldIndex,
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
#endif //  DATASETS_HEADERS_FILESTRUCTURES_H_
