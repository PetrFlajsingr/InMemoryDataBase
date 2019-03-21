//
// Created by Petr Flajsingr on 14/11/2018.
//

#ifndef CSV_READER_SORTSTRUCTURES_H
#define CSV_READER_SORTSTRUCTURES_H

#include <cstdint>
#include <Types.h>
#include <vector>
namespace DataSets {

struct SortItem {
  BaseField *field;
  SortOrder order;
};

struct SortOptions {
  std::vector<SortItem> options;

  void addOption(BaseField *field, SortOrder order) {
    options.emplace_back(SortItem{field, order});
  }
};

}  // namespace DataSets

#endif //CSV_READER_SORTSTRUCTURES_H
