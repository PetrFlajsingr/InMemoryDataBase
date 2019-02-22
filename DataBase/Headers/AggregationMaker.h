//
// Created by Petr Flajsingr on 2019-02-21.
//

#ifndef PROJECT_AGGREGATONMAKER_H
#define PROJECT_AGGREGATONMAKER_H

#include <QueryCommon.h>
#include <BaseField.h>
#include <MemoryDataSet.h>

namespace DataBase {
class Table;
class View;

struct Unique {
  DataSets::BaseField *field;
  DataContainer lastVal;

  explicit Unique(DataSets::BaseField *field);

  bool check(const DataContainer &newVal);
};

struct Sum {
  DataSets::BaseField *field;
  DataContainer sum;

  explicit Sum(DataSets::BaseField *field);

  void accumulate(const DataContainer &val);
  void reset();
};

class AggregationMaker {
 public:
  explicit AggregationMaker(const std::shared_ptr<Table> &table);
  explicit AggregationMaker(const std::shared_ptr<View> &view);

  std::shared_ptr<Table> aggregate(const StructuredQuery &structuredQuery);
 private:
  std::shared_ptr<Table> table = nullptr;
  std::shared_ptr<View> view = nullptr;

  std::shared_ptr<DataSets::MemoryDataSet> prepareDataSet(const DataBase::StructuredQuery &structuredQuery);

  std::shared_ptr<DataSets::MemoryDataSet> aggregateDataSet(DataSets::MemoryDataSet *ds,
                                                            DataSets::MemoryDataSet *result);
  std::shared_ptr<DataSets::MemoryDataSet> aggregateView(DataSets::MemoryViewDataSet *ds,
                                                         DataSets::MemoryDataSet *result);

  std::vector<Unique> groupByFields;
  std::vector<Sum> sumFields;
  std::vector<std::pair<AgrOperator, gsl::index>> writeOrder;
};
}

#endif //PROJECT_AGGREGATONMAKER_H
