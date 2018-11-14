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
  uint64_t fieldIndex;
  SortOrder order;
};

struct SortOptions {
  std::vector<SortItem> options;

  void addOption(uint64_t index, SortOrder order) {
    SortItem item {
      index,
      order};
    options.push_back(item);
  }
};

}  // namespace DataSets

#endif //CSV_READER_SORTSTRUCTURES_H
