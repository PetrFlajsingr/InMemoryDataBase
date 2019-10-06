//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_HEADERS_FILTERSTRUCTURES_H_
#define DATASETS_HEADERS_FILTERSTRUCTURES_H_

#include "Types.h"
#include <BaseField.h>
#include <QueryCommon.h>
#include <Utilities.h>
#include <algorithm>
#include <string>
#include <vector>

namespace DataSets {
/**
 * Types of filter operations.
 */
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
/**
 * Single item to filter by.
 */
struct FilterItem {
  const DataSets::BaseField *field;
  std::vector<DataContainer> searchData;
  FilterOption filterOption;

  FilterItem(const BaseField *field, std::vector<DataContainer> searchData, FilterOption filterOption);
};
/**
 * Structure holding options for filtering.
 */
struct FilterOptions {
  std::vector<FilterItem> options;

  void addOption(const DataSets::BaseField *field, const std::vector<DataContainer> &values,
                 const FilterOption &filterOption);

  void addOption(const DataSets::BaseField *field, const std::vector<std::string> &values,
                 const FilterOption &filterOption);
};
} // namespace DataSets

#endif // DATASETS_HEADERS_FILTERSTRUCTURES_H_
