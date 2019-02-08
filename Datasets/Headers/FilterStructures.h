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
#include <DateTimeUtils.h>

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
  const DataSets::BaseField *field;
  std::vector<DataContainer> searchData;
  FilterOption filterOption;

  FilterItem(const BaseField *field,
             const std::vector<DataContainer> &searchData,
             FilterOption filterOption)
      : field(field), searchData(searchData), filterOption(filterOption) {}

  virtual ~FilterItem() {
    /*switch (field->getFieldType()) {
      case ValueType::String:
        for (const auto &data : searchData) {
          delete[] data._string;
        }
        break;
      case ValueType::Currency:
        for (const auto &data : searchData) {
          delete data._currency;
        }
        break;
      case ValueType::DateTime:
        for (const auto &data : searchData) {
          delete data._currency;
        }
        break;
    }*/
  }
};

struct FilterOptions {
  std::vector<FilterItem> options;

  void addOption(const DataSets::BaseField *field,
                 const std::vector<DataContainer> &searchString,
                 const FilterOption filterOption) {
    options.emplace_back(FilterItem{field,
                                    searchString,
                                    filterOption});
  }

  void addOption(const DataSets::BaseField *field,
                 const std::vector<std::string> &values,
                 const FilterOption filterOption) {
    std::vector<DataContainer> searchVals;

    std::transform(values.begin(),
                   values.end(),
                   std::back_inserter(searchVals),
                   [field](std::string_view str) {
                     switch (field->getFieldType()) {
                       case ValueType::Integer:
                         return DataContainer{._integer = Utilities::stringToInt(
                             str)};
                       case ValueType::Double:
                         return DataContainer{._double = Utilities::stringToDouble(
                             str)};
                       case ValueType::String:
                         return DataContainer{._string = Utilities::copyStringToNewChar(
                             str)};
                       case ValueType::Currency:
                         return DataContainer{._currency = new Currency(std::string(
                             str))};
                       case ValueType::DateTime:
                         return DataContainer{._dateTime = new DateTime(str)};
                       default:
                         throw IllegalStateException(
                             "Internal error. FilterOptions::addOption");
                     }
                   });

    options.emplace_back(field, searchVals, filterOption);
  }
};
}  // namespace DataSets

#endif  // DATASETS_HEADERS_FILTERSTRUCTURES_H_
