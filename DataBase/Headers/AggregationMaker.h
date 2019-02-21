//
// Created by Petr Flajsingr on 2019-02-21.
//

#ifndef PROJECT_AGGREGATONMAKER_H
#define PROJECT_AGGREGATONMAKER_H

#include <QueryCommon.h>

namespace DataBase {
class Table;
class View;
class AggregationMaker {
 public:
  explicit AggregationMaker(const std::shared_ptr<Table> &table);
  explicit AggregationMaker(const std::shared_ptr<View> &view);

  std::shared_ptr<Table> aggregate(const StructuredQuery &structuredQuery);
 private:
  std::shared_ptr<Table> table = nullptr;
  std::shared_ptr<View> view = nullptr;
};
}

#endif //PROJECT_AGGREGATONMAKER_H
