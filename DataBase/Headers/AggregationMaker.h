//
// Created by Petr Flajsingr on 2019-02-21.
//

#ifndef PROJECT_AGGREGATONMAKER_H
#define PROJECT_AGGREGATONMAKER_H

#include <QueryCommon.h>
#include <BaseField.h>
#include <MemoryDataSet.h>

namespace DataBase {
struct Table;
struct View;

struct BaseAgr {
  std::pair<gsl::index, gsl::index> fieldIndex;
  DataSets::BaseField *field;
  explicit BaseAgr(DataSets::BaseField *field);
};

struct Unique : public BaseAgr {
  DataContainer lastVal;
  explicit Unique(DataSets::BaseField *field);
  bool check(const DataContainer &newVal);
};

struct Sum : public BaseAgr {
  DataContainer sum;
  explicit Sum(DataSets::BaseField *field);
  void accumulate(const DataContainer &val);
  void reset();
};

struct Min : public BaseAgr {
  DataContainer min;
  explicit Min(DataSets::BaseField *field);
  void check();
  void reset();
};

struct Max : public BaseAgr {
  DataContainer max;
  explicit Max(DataSets::BaseField *field);
  void check();
  void reset();
};

struct Count : public BaseAgr {
  uint64_t count = 0;
  explicit Count(DataSets::BaseField *field);
  void add();
  void reset();
};

struct Avg : public Sum, public Count {
  explicit Avg(DataSets::BaseField *field);
  Avg(DataSets::BaseField *field, DataSets::BaseField *field1);
  void accumulate(const DataContainer &val);
  void reset();
  DataContainer getResult();
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

  void aggregateDataSet(DataSets::MemoryDataSet *ds,
                        DataSets::MemoryDataSet *result);
  void aggregateView(DataSets::MemoryViewDataSet *ds,
                     DataSets::MemoryDataSet *result);

  std::vector<Unique> groupByFields;
  std::vector<Sum> sumFields;
  std::vector<Count> countFields;
  std::vector<std::pair<AgrOperator, gsl::index>> writeOrder;

  std::vector<std::pair<gsl::index, gsl::index>> fieldIndices;
};
}

#endif //PROJECT_AGGREGATONMAKER_H
