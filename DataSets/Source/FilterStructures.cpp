//
// Created by Petr Flajsingr on 2019-02-19.
//

#include <FilterStructures.h>

DataSets::FilterOption DataSets::condOpToFilterOp(DataBase::CondOperator op) {
  switch (op) {
    case DataBase::CondOperator::greater:return FilterOption::Greater;
    case DataBase::CondOperator::greaterEqual:return FilterOption::GreaterEqual;
    case DataBase::CondOperator::less:return FilterOption::Less;
    case DataBase::CondOperator::lessEqual:return FilterOption::LessEqual;
    case DataBase::CondOperator::equal:return FilterOption::Equals;
    case DataBase::CondOperator::notEqual:return FilterOption::NotEquals;
  }
}

DataSets::FilterItem::FilterItem(const DataSets::BaseField *field,
                                 const std::vector<DataContainer> &searchData,
                                 DataSets::FilterOption filterOption)
    : field(field), searchData(searchData), filterOption(filterOption) {}

void DataSets::FilterOptions::addOption(const DataSets::BaseField *field,
                                        const std::vector<DataContainer> &searchString,
                                        const DataSets::FilterOption filterOption) {
  options.emplace_back(FilterItem{field,
                                  searchString,
                                  filterOption});
}

void DataSets::FilterOptions::addOption(const DataSets::BaseField *field,
                                        const std::vector<std::string> &values,
                                        const DataSets::FilterOption filterOption) {
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