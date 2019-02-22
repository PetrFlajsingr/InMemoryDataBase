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
  auto result = prepareDataSet(structuredQuery);

  DataSets::SortOptions options;
  std::for_each(groupByFields.begin(), groupByFields.end(),
                [&options](Unique &un) {
                  options.addOption(un.field, SortOrder::Ascending);
                });
  if (view != nullptr) {
    auto workDs = view->dataSet.get();
    workDs->sort(options);

    aggregateView(workDs, result.get());
  } else {
    auto workDs = table->dataSet.get();
    workDs->sort(options);

    aggregateDataSet(workDs, result.get());
  }

  return std::make_shared<Table>(result);
}
std::shared_ptr<DataSets::MemoryDataSet> DataBase::AggregationMaker::prepareDataSet(
    const DataBase::StructuredQuery &structuredQuery) {
  DataSets::BaseDataSet *dataSet;
  if (view != nullptr) {
    dataSet = dynamic_cast<DataSets::BaseDataSet *>(view->dataSet.get());
  } else {
    dataSet = dynamic_cast<DataSets::BaseDataSet *>(table->dataSet.get());
  }
  groupByFields.clear();
  sumFields.clear();
  writeOrder.clear();

  std::vector<std::string> fieldNames;
  std::vector<ValueType> fieldTypes;
  for (const auto &val : structuredQuery.agr.data) {
    auto field = dataSet->fieldByName(val.field.column);
    switch (val.op) {
      case AgrOperator::group:groupByFields.emplace_back(field);
        writeOrder.emplace_back(AgrOperator::group, groupByFields.size() - 1);
        break;
      case AgrOperator::min:
        //minFields.emplace_back(field);
        //writeOrder.emplace_back(AgrOperator::min, minFields.size() - 1);
        break;
      case AgrOperator::max:
        //maxFields.emplace_back(field);
        //writeOrder.emplace_back(AgrOperator::max, maxFields.size() - 1);
        break;
      case AgrOperator::count:
        //countFields.emplace_back(field);
        //writeOrder.emplace_back(AgrOperator::count, countFields.size() - 1);
        break;
      case AgrOperator::sum:sumFields.emplace_back(field);
        writeOrder.emplace_back(AgrOperator::sum, sumFields.size() - 1);
        break;
      case AgrOperator::avg:
        //avgFields.emplace_back(field);
        //writeOrder.emplace_back(AgrOperator::avg, avgFields.size() - 1);
        break;
    }
    fieldNames.emplace_back(field->getName());
  }

  auto result = std::make_shared<DataSets::MemoryDataSet>("");
  result->openEmpty(fieldNames, fieldTypes);

  return result;
}
std::shared_ptr<DataSets::MemoryDataSet> DataBase::AggregationMaker::aggregateDataSet(
    DataSets::MemoryDataSet *ds,
    DataSets::MemoryDataSet *result) {
  bool write = false;
  for (const auto &val : *ds) {
    for (auto &groupField : groupByFields) {
      if (groupField.check((*val)[groupField.field->getIndex()])) {
        write = true;
        break;
      }
    }

    if (write) {
      write = false;
      result->append();
      auto resultIter = result->end() - 1;
      int cnt = 0;
      for (const auto &info : writeOrder) {
        switch (info.first) {
          case AgrOperator::sum:
            switch (sumFields[info.second].field->getFieldType()) {
              case ValueType::Integer:
                (*(*resultIter))[cnt].copyFrom<int>(sumFields[info.second].sum);
                break;
              case ValueType::Double:
                (*(*resultIter))[cnt].copyFrom<double>(sumFields[info.second].sum);
                break;
              case ValueType::Currency:
                (*(*resultIter))[cnt].copyFrom<Currency>(sumFields[info.second].sum);
                break;
            }
            break;
          case AgrOperator::avg:break;
          case AgrOperator::count:break;
          case AgrOperator::min:break;
          case AgrOperator::max:break;
          case AgrOperator::group:
            switch (groupByFields[info.second].field->getFieldType()) {
              case ValueType::Integer:
                (*(*resultIter))[cnt].copyFrom<int>(groupByFields[info.second].lastVal);
                break;
              case ValueType::Double:
                (*(*resultIter))[cnt].copyFrom<double>(groupByFields[info.second].lastVal);
                break;
              case ValueType::String:
                (*(*resultIter))[cnt].copyFrom<gsl::zstring<>>(groupByFields[info.second].lastVal);
                break;
              case ValueType::Currency:
                (*(*resultIter))[cnt].copyFrom<Currency>(groupByFields[info.second].lastVal);
                break;
              case ValueType::DateTime:
                (*(*resultIter))[cnt].copyFrom<DateTime>(groupByFields[info.second].lastVal);
                break;
            }
            break;
        }
        ++cnt;
      }
      for (auto &unique : groupByFields) {
        switch (unique.field->getFieldType()) {
          case ValueType::Integer: unique.lastVal.copyFrom<int>((*val)[unique.field->getIndex()]);
            break;
          case ValueType::Double: unique.lastVal.copyFrom<double>((*val)[unique.field->getIndex()]);
            break;
          case ValueType::String: unique.lastVal.copyFrom<gsl::zstring<>>((*val)[unique.field->getIndex()]);
            break;
          case ValueType::Currency: unique.lastVal.copyFrom<Currency>((*val)[unique.field->getIndex()]);
            break;
          case ValueType::DateTime: unique.lastVal.copyFrom<DateTime>((*val)[unique.field->getIndex()]);
            break;
        }
      }
      for (auto &sum : sumFields) {
        sum.reset();
      }
    }

    for (auto &sum : sumFields) {
      sum.accumulate((*val)[sum.field->getIndex()]);
    }
  }
}
std::shared_ptr<DataSets::MemoryDataSet> DataBase::AggregationMaker::aggregateView(
    DataSets::MemoryViewDataSet *ds,
    DataSets::MemoryDataSet *result) {
  throw NotImplementedException();
}

bool DataBase::Unique::check(const DataContainer &newVal) {
  int cmpResult;
  switch (field->getFieldType()) {
    case ValueType::Integer:cmpResult = lastVal.compare<int>(lastVal);
      break;
    case ValueType::Double:cmpResult = lastVal.compare<double>(lastVal);
      break;
    case ValueType::String:cmpResult = lastVal.compare<gsl::zstring<>>(lastVal);
      break;
    case ValueType::Currency:cmpResult = lastVal.compare<Currency>(lastVal);
      break;
    case ValueType::DateTime:cmpResult = lastVal.compare<DateTime>(lastVal);
      break;
  }
  if (cmpResult == 0) {
    lastVal = newVal;
    return true;
  }
  return false;
}
DataBase::Unique::Unique(DataSets::BaseField *field) : field(field) {
  switch (field->getFieldType()) {
    case ValueType::String: lastVal = "";
      break;
    case ValueType::Currency: lastVal = new Currency();
      break;
    case ValueType::DateTime: lastVal = new DateTime();
      break;
  }
}
DataBase::Sum::Sum(DataSets::BaseField *field) : field(field) {
  if (field->getFieldType() != ValueType::Integer
      && field->getFieldType() != ValueType::Double
      && field->getFieldType() != ValueType::Currency) {
    throw std::logic_error("DataBase::Sum constructor.");
  }
}

void DataBase::Sum::accumulate(const DataContainer &val) {
  switch (field->getFieldType()) {
    case ValueType::Integer: sum._integer += val._integer;
      break;
    case ValueType::Double: sum._double += val._double;
      break;
    case ValueType::Currency: *sum._currency += *val._currency;
      break;
  }
}
void DataBase::Sum::reset() {
  switch (field->getFieldType()) {
    case ValueType::Integer: sum = 0;
      break;
    case ValueType::Double: sum = 0.0;
      break;
    case ValueType::Currency: sum = Currency(0);
      break;
  }
}
