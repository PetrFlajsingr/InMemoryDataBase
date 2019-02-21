//
// Created by Petr Flajsingr on 2019-02-21.
//

#include <AggregationMaker.h>
#include <BaseDataSet.h>
#include <MemoryDataBase.h>

DataBase::AggregationMaker::AggregationMaker(const std::shared_ptr<DataBase::Table> &table)
    : table(table) {}

DataBase::AggregationMaker::AggregationMaker(const std::shared_ptr<DataBase::View> &view)
    : view(view) {}

std::shared_ptr<DataBase::Table> DataBase::AggregationMaker::aggregate(const DataBase::StructuredQuery &structuredQuery) {
  DataSets::BaseDataSet *dataSet;
  if (view != nullptr) {
    dataSet = dynamic_cast<DataSets::BaseDataSet *>(view->dataSet.get());
  } else {
    dataSet = dynamic_cast<DataSets::BaseDataSet *>(table->dataSet.get());
  }
  std::vector<DataSets::BaseField *> groupByFields;
  std::vector<std::pair<DataContainer, DataSets::BaseField *>> minFields;
  std::vector<std::pair<DataContainer, DataSets::BaseField *>> maxFields;
  std::vector<std::pair<int, DataSets::BaseField *>> countFields;
  std::vector<std::pair<DataContainer, DataSets::BaseField *>> sumFields;
  std::vectorstd::tuple<DataContainer, int < DataSets::BaseField * >>
  avgFields;

  std::vector<std::string> fieldNames;
  std::vector<ValueType> fieldTypes;
  for (const auto &val : structuredQuery.agr.data) {
    auto field = dataSet->fieldByName(val.field.column);
    switch (val.op) {
      case AgrOperator::group:groupByFields.emplace_back(field);
        break;
      case AgrOperator::min:minFields.emplace_back(field);
        break;
      case AgrOperator::max:maxFields.emplace_back(field);
        break;
      case AgrOperator::count:countFields.emplace_back(field);
        break;
      case AgrOperator::sum:sumFields.emplace_back(field);
        break;
      case AgrOperator::avg:avgFields.emplace_back(field);
        break;
    }
    fieldNames.emplace_back(field->getName());
  }

  auto result = std::make_shared<DataSets::MemoryDataSet>("intermmediate");
  result->openEmpty(fieldNames, fieldTypes);

  dataSet->resetBegin();
  while (dataSet->next()) {
    // TODO
  }

  return result;
}
