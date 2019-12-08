//
// Created by Petr Flajsingr on 14/11/2018.
//

#ifndef CSV_READER_SORTSTRUCTURES_H
#define CSV_READER_SORTSTRUCTURES_H

#include <Types.h>
#include <cstdint>
#include <vector>
namespace DataSets {

struct SortItem {
  BaseField *field;
  SortOrder order;
};

struct SortOptions {
  std::vector<SortItem> options;

  SortOptions & addOption(BaseField *field, SortOrder order) { options.emplace_back(SortItem{field, order}); return *this;}
};

} // namespace DataSets

#endif // CSV_READER_SORTSTRUCTURES_H
