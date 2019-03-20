//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_HEADERS_FILTERSTRUCTURES_H_
#define DATASETS_HEADERS_FILTERSTRUCTURES_H_

#include <vector>
#include <algorithm>
#include <string>
#include <BaseField.h>
#include <Utilities.h>
#include "Types.h"
#include <QueryCommon.h>

// TODO: predelat tak, aby podporoval double, integer, currency...
// TODO: predelat pro prehlednost - vytvorit DataContainer for FilterOptions uvnitr automaticky
//  - pretizena funkce pro vsechny mozne hodnoty?
namespace DataSets {
enum class FilterOption {
  Equals,
  NotEquals,
  StartsWith,
  Contains,
  EndsWith,
  NotContains,
  NotStartsWith,
  NotEndsWith,
  Greater,
  GreaterEqual,
  Less,
  LessEqual,
};

FilterOption condOpToFilterOp(DataBase::CondOperator op);

struct FilterItem {
  const DataSets::BaseField *field;
  std::vector<DataContainer> searchData;
  FilterOption filterOption;

  FilterItem(const BaseField *field,
             const std::vector<DataContainer> &searchData,
             FilterOption filterOption);
};

struct FilterOptions {
  std::vector<FilterItem> options;

  void addOption(const DataSets::BaseField *field,
                 const std::vector<DataContainer> &searchString,
                 const FilterOption filterOption);

  void addOption(const DataSets::BaseField *field,
                 const std::vector<std::string> &values,
                 const FilterOption filterOption);
};
}  // namespace DataSets

#endif  // DATASETS_HEADERS_FILTERSTRUCTURES_H_
