//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_HEADERS_FILTERSTRUCTURES_H_
#define DATASETS_HEADERS_FILTERSTRUCTURES_H_

#include <vector>
#include <string>
#include "Types.h"

// TODO: predelat tak, aby podporoval double, integer, currency...
// TODO: predelat pro prehlednost - vytvorit DataContainer for FilterOptions uvnitr automaticky
//  - pretizena funkce pro vsechny mozne hodnoty?
namespace DataSets {
enum class FilterOption {
  Equals,
  StartsWith,
  Contains,
  EndsWith,
  NotContains,
  NotStartsWith,
  NotEndsWith
};

struct FilterItem {
  uint64_t fieldIndex;
  ValueType type;
  std::vector<DataContainer> searchData;
  FilterOption filterOption;
};

struct FilterOptions {
  std::vector<FilterItem> options;

  void addOption(const uint64_t fieldIndex,
                 const ValueType type,
                 const std::vector<DataContainer> &searchString,
                 const FilterOption filterOption) {
    FilterItem item{
        fieldIndex,
        type,
        searchString,
        filterOption
    };

    options.push_back(item);
  }
};
}  // namespace DataSets

#endif  // DATASETS_HEADERS_FILTERSTRUCTURES_H_
