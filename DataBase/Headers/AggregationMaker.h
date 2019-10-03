//
// Created by Petr Flajsingr on 2019-02-21.
//

#ifndef PROJECT_AGGREGATONMAKER_H
#define PROJECT_AGGREGATONMAKER_H

#include <QueryCommon.h>
#include <BaseField.h>
#include <MemoryDataSet.h>
#include <AggregationStructures.h>

namespace DataBase {
struct Table;
struct View;

/**
 * Class doing aggregation on views/tables.
 */
class AggregationMaker {
 public:
  /**
   *
   * @param table table to aggregate
   */
  explicit AggregationMaker(std::shared_ptr<Table> table);
  /**
   *
   * @param view view to aggregate
   */
  explicit AggregationMaker(std::shared_ptr<View> view);
  /**
   * Do aggregation operation defined in query.
   * @param structuredQuery query containing info about required aggregation operations
   * @return Table containing aggregated values
   */
  std::shared_ptr<Table> aggregate(const StructuredQuery &structuredQuery);

 private:
  std::shared_ptr<Table> table = nullptr;
  std::shared_ptr<View> view = nullptr;

  std::shared_ptr<DataSets::MemoryDataSet> prepareDataSet(const DataBase::StructuredQuery &structuredQuery);

  void aggregateDataSet(DataSets::MemoryDataSet *ds, DataSets::MemoryDataSet *result);
  void aggregateView(DataSets::MemoryViewDataSet *ds, DataSets::MemoryDataSet *result);

  std::vector<Unique> groupByFields;
  std::vector<Sum> sumFields;
  std::vector<Count> countFields;
  std::vector<std::pair<AgrOperator, gsl::index>> writeOrder;

  std::vector<std::pair<gsl::index, gsl::index>> fieldIndices;
};
}

#endif //PROJECT_AGGREGATONMAKER_H
