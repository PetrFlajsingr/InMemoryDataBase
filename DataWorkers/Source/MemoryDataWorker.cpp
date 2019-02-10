//
// Created by Petr Flajsingr on 12/11/2018.
//
#include <MemoryDataWorker.h>

DataWorkers::MemoryDataWorker::MemoryDataWorker(std::shared_ptr<DataSets::BaseDataSet> dataSet) {
  this->dataset = dataSet;
}

std::vector<std::string> DataWorkers::MemoryDataWorker::getMultiChoiceNames() {
  return columnChoices;
}

std::vector<std::string> DataWorkers::MemoryDataWorker::getChoices(std::string choiceName) {
  auto field = dataset->fieldByName(choiceName);

  DataSets::SortOptions sortOptions;
  sortOptions.addOption(field, SortOrder::Ascending);
  dataset->sort(sortOptions);

  std::string value;
  std::vector<std::string> result;

  while (dataset->next()) {
    if (field->getAsString() != value) {
      value = field->getAsString();
      result.emplace_back(value);
    }
  }

  return result;
}

DataWorkers::MemoryDataWorker::MemoryDataWorker(
    DataProviders::BaseDataProvider &dataProvider,
    const std::vector<ValueType> &fieldTypes)
    : BaseDataWorker() {
  dataset = std::make_shared<DataSets::MemoryDataSet>("main");

  dataset->open(dataProvider, fieldTypes);
}

// TODO: fix pro filter view -- nebo rovnou pozdeji smazat cely data worker...
void DataWorkers::MemoryDataWorker::writeResult(DataWriters::BaseDataWriter &writer,
                                                std::string_view sql) {
  queryData = SQLParser::parse(sql);

  writeHeaders(writer);

  //  rozrazeni podle distinct hodnot
  DataSets::SortOptions sortOptions;
  for (gsl::index i = 0;
       i < dataset->getFieldNames().size()
           && i < queryData.projections.size();
       ++i) {
    if (queryData.projections[i].tableName == "main" &&
        queryData.projections[i].operation == Operation::Distinct) {
      sortOptions.addOption(
          dataset->fieldByName(queryData.projections[i].columnName),
          SortOrder::Ascending);
    }
  }

  auto tmpDataSet = dataset;
  //  vyfiltrovani nechtenych zaznamu
  if (!queryData.selections.empty()) {
    dataset = std::dynamic_pointer_cast<DataSets::BaseDataSet>(filterDataSet());
  }

  std::thread mainDataSetSortThread
      (&MemoryDataWorker::T_ThreadSort, this, std::ref(sortOptions));

  bool doJoin = !queryData.joins.empty();
  std::vector<InnerJoinStructure> joinStructures;

  //  akumulatory pro vysveldky pro kazdy radek
  std::vector<ResultAccumulator *> accumulators;
  for (auto &op : queryData.projections) {
    if (op.tableName == "main") {
      accumulators.emplace_back(
          new ResultAccumulator(dataset->fieldByName(op.columnName),
                                op.operation));
    }
  }

  //  seradeni pripojenych datasetu
  if (doJoin) {
    for (int i = 0; i < queryData.joins.size(); ++i) {
      InnerJoinStructure joinStructure;
      joinStructure.fieldAddi =
          additionalDataSets[i]->fieldByName(queryData.joins[i].column2Name);

      std::string searchedName = queryData.joins[i].column1Name;
      joinStructure.fieldMain =
          dataset->fieldByIndex(std::find_if(accumulators.begin(),
                                             accumulators.end(),
                                             [&searchedName](ResultAccumulator *acc) {
                                               return acc->getName()
                                                   == searchedName;
                                             }) - accumulators.begin());
      for (auto &proj : queryData.projections) {
        if (proj.tableName == queryData.joins[i].table2Name) {
          joinStructure.projectFields.emplace_back(additionalDataSets[i]->fieldByName(
              proj.columnName));
        }
      }

      if (joinStructure.projectFields.empty()) {
        continue;
      }
      joinStructures.emplace_back(joinStructure);
      DataSets::SortOptions options;
      options.addOption(joinStructure.fieldAddi,
                        SortOrder::Ascending);
      additionalDataSets[i]->sort(options);
      additionalDataSets[i]->first();
    }
  }

  mainDataSetSortThread.join();

  bool savedResults = false, writeResults = false;
  std::vector<std::string> results;
  results.reserve(queryData.projections.size());
  while (dataset->next()) {
    for (auto accumulator : accumulators) {
      if (accumulator->step() && !savedResults) {
        for (auto toSave : accumulators) {
          results.emplace_back(toSave->getResult());
        }

        //  pokud je pozadovan join, je nutne hledat v dalsich tabulkach
        if (doJoin) {
          for (gsl::index i = 0; i < joinStructures.size(); ++i) {
            DataContainer container =
                accumulators[joinStructures[i].fieldMain->getIndex()]->getContainer();

            DataSets::FilterItem item{
                joinStructures[i].fieldAddi,
                {container},
                DataSets::FilterOption::Equals
            };
            if (additionalDataSets[i]->findFirst(item)) {
              writeResults = true;
              for (auto field : joinStructures[i].projectFields) {
                results.emplace_back(field->getAsString());
              }
            } else if (queryData.joins[i].joinType == JoinType::NormalJoin) {
              // zrus vypisovani zaznamu pokud k nemu neni nalezen zaznam k napojeni
              writeResults = false;
              break;
            } else {
              writeResults = true;
              for (auto field : joinStructures[i].projectFields) {
                results.emplace_back("(NULL)");
              }
            }
          }
        } else {
          writeResults = true;
        }

        if (writeResults) {
          writer.writeRecord(results);
        }
        results.clear();
        savedResults = true;
      }
      if (savedResults) {
        accumulator->reset();
      }
    }
    savedResults = false;
  }

  for (auto toSave : accumulators) {
    results.emplace_back(toSave->getResultForce());
  }

  if (doJoin) {
    for (int i = 0; i < joinStructures.size(); ++i) {
      ValueType type =
          joinStructures[i].fieldMain->getFieldType();
      DataContainer
          container =
          accumulators[joinStructures[i].fieldMain->getIndex()]->getContainer();

      DataSets::FilterItem item {
          joinStructures[i].fieldAddi,
          {container},
          DataSets::FilterOption::Equals
      };
      if (additionalDataSets[i]->findFirst(item)) {
        writeResults = true;
        for (auto field : joinStructures[i].projectFields) {
          results.emplace_back(field->getAsString());
        }
      } else if (queryData.joins[i].joinType == JoinType::NormalJoin) {
        // zrus vypisovani zaznamu pokud k nemu neni nalezen zaznam k napojeni
        writeResults = false;
        break;
      } else {
        writeResults = true;
        for (auto field : joinStructures[i].projectFields) {
          results.emplace_back("(NULL)");
        }
      }
    }
  }

  if (writeResults) {
    writer.writeRecord(results);
  }

  for (auto *acc : accumulators) {
    delete acc;
  }

  dataset = tmpDataSet;
}

void DataWorkers::MemoryDataWorker::writeHeaders(DataWriters::BaseDataWriter &writer) {
  std::vector<std::string> header;

  std::transform(queryData.projections.begin(),
                 queryData.projections.end(),
                 std::back_inserter(header),
                 [](const ProjectionOperation &op) {
                   return op.columnName + OperationToString(op.operation);
                 });

  writer.writeHeader(header);
}

std::shared_ptr<DataSets::ViewDataSet> DataWorkers::MemoryDataWorker::filterDataSet() {
  DataSets::FilterOptions filterOptions;

  for (auto &selection : queryData.selections) {
    if (selection.tableName == "main") {
      auto field = dataset->fieldByName(selection.columnName);
      auto type = field->getFieldType();
      std::vector<DataContainer> filterSelection;

      DataContainer container;
      for (std::string &value : selection.reqs) {
        switch (type) {
          case ValueType::String:container._string = strdup(value.c_str());
            break;
          case ValueType::Integer:
            container._integer = Utilities::stringToInt(value);
            break;
          case ValueType::Double:
            container._double = Utilities::stringToDouble(value);
            break;
          case ValueType::Currency:
            *container._currency = dec::fromString<Currency>(value);
            break;
          case ValueType::DateTime:container._dateTime = new DateTime(value);
            break;
        }
        filterSelection.emplace_back(container);
      }

      filterOptions.addOption(
          field,
          filterSelection,
          DataSets::FilterOption::Equals);
    }
  }

  return dataset->filter(filterOptions);
}

void DataWorkers::MemoryDataWorker::T_ThreadSort(DataSets::SortOptions &sortOptions) {
  dataset->sort(sortOptions);
}

DataWorkers::ResultAccumulator::ResultAccumulator(DataSets::BaseField *field,
                                                  Operation op)
    : field(field), operation(op) {
  switch (field->getFieldType()) {
    case ValueType::String:data._string = nullptr;
      break;
    case ValueType::Integer:data._integer = 0;
      break;
    case ValueType::Double:data._double = 0.0;
      break;
    case ValueType::Currency:data._currency = new Currency();
      previousData._currency = new Currency();
      break;
    case ValueType::DateTime:data._dateTime = new DateTime();
      previousData._dateTime = new DateTime();
      break;
  }
}

bool DataWorkers::ResultAccumulator::handleDistinct() {
  switch (field->getFieldType()) {
    case ValueType::Integer: {
      int value = reinterpret_cast<DataSets::IntegerField *>(field)
          ->getAsInteger();
      if (!firstDone) {
        firstDone = true;
        data._integer = value;
        return false;
      }
      if (value != data._integer) {
        previousData._integer = data._integer;
        result = std::to_string(data._integer);
        data._integer = value;
        distinct = true;
        return true;
      }
      break;
    }
    case ValueType::Double: {
      double value = reinterpret_cast<DataSets::DoubleField *>(field)
          ->getAsDouble();
      if (!firstDone) {
        firstDone = true;
        data._double = value;
        return false;
      }
      if (value != data._double) {
        previousData._double = data._double;
        result = std::to_string(data._double);
        data._double = value;
        distinct = true;
        return true;
      }
      break;
    }
    case ValueType::String: {
      auto value = field->getAsString();
      if (!firstDone) {
        firstDone = true;
        data._string = Utilities::copyStringToNewChar(value);
        return false;
      }
      if (value != data._string) {
        delete[] previousData._string;
        previousData._string = data._string;
        result = data._string;
        data._string = Utilities::copyStringToNewChar(value);
        distinct = true;
        return true;
      }
      break;
    }
    case ValueType::Currency: {
      Currency value = reinterpret_cast<DataSets::CurrencyField *>(field)
          ->getAsCurrency();
      if (!firstDone) {
        firstDone = true;
        *data._currency = value;
        return false;
      }
      if (value != *data._currency) {
        *previousData._currency = *data._currency;
        result = dec::toString(*data._currency);
        *data._currency = value;
        distinct = true;
        return true;
      }
      break;
    }
    case ValueType::DateTime: {
      DateTime value = reinterpret_cast<DataSets::DateTimeField *>(field)
          ->getAsDateTime();
      if (!firstDone) {
        firstDone = true;
        *data._dateTime = value;
        return false;
      }
      if (value != *data._dateTime) {
        *previousData._currency = *data._currency;
        result = dec::toString(*data._currency);
        *data._dateTime = value;
        distinct = true;
        return true;
      }
      break;
    }
  }
  return false;
}

bool DataWorkers::ResultAccumulator::handleSum() {
  switch (field->getFieldType()) {
    case ValueType::Integer: {
      int value = reinterpret_cast<DataSets::IntegerField *>(field)
          ->getAsInteger();
      data._integer += value;
      break;
    }
    case ValueType::Double: {
      double value = reinterpret_cast<DataSets::DoubleField *>(field)
          ->getAsDouble();
      data._double += value;
      break;
    }
    case ValueType::String: {
      throw IllegalStateException(
          "Internal error. DataWorkers::ResultAccumulator::handleSum() on String");
    }
    case ValueType::Currency: {
      Currency value = reinterpret_cast<DataSets::CurrencyField *>(field)
          ->getAsCurrency();
      *data._currency += value;
      break;
    }
    case ValueType::DateTime:
      throw IllegalStateException(
          "Internal error. DataWorkers::ResultAccumulator::handleSum() on DateTime");
  }
  return false;
}

bool DataWorkers::ResultAccumulator::handleAverage() {
  dataCount++;
  switch (field->getFieldType()) {
    case ValueType::Integer: {
      int value = reinterpret_cast<DataSets::IntegerField *>(field)
          ->getAsInteger();
      data._integer += value;
      break;
    }
    case ValueType::Double: {
      double value = reinterpret_cast<DataSets::DoubleField *>(field)
          ->getAsDouble();
      data._double += value;
      break;
    }
    case ValueType::String: {
      throw IllegalStateException(
          "Internal error. DataWorkers::ResultAccumulator::handleAverage() on string");
    }
    case ValueType::Currency: {
      Currency value = reinterpret_cast<DataSets::CurrencyField *>(field)
          ->getAsCurrency();
      *data._currency += value;
      break;
    }
    case ValueType::DateTime: {
      throw IllegalStateException(
          "Internal error. DataWorkers::ResultAccumulator::handleAverage() on DateTime");
    }
  }
  return false;
}

std::string DataWorkers::ResultAccumulator::resultSum() {
  switch (field->getFieldType()) {
    case ValueType::Integer: {
      std::string result = std::to_string(data._integer);
      data._integer = 0;
      return result;
    }
    case ValueType::Double: {
      std::string result = std::to_string(data._double);
      data._double = 0;
      return result;
    }
    case ValueType::Currency: {
      std::string result = dec::toString(*data._currency);
      *data._currency = 0;
      return result;
    }
    default:
      throw IllegalStateException(
          "Internal error. DataWorkers::ResultAccumulator::resultSum()");
  }
}

std::string DataWorkers::ResultAccumulator::resultAverage() {
  switch (field->getFieldType()) {
    case ValueType::Integer: {
      std::string result = std::to_string(data._integer / dataCount);
      data._integer = 0;
      return result;
    }
    case ValueType::Double: {
      std::string result = std::to_string(data._double / dataCount);
      data._double = 0;
      return result;
    }
    case ValueType::Currency: {
      std::string result = dec::toString(*data._currency / Currency(dataCount));
      *data._currency = 0;
      return result;
    }
    default:
      throw IllegalStateException(
          "Internal error. DataWorkers::ResultAccumulator::resultSum()");
  }
}

bool DataWorkers::ResultAccumulator::step() {
  switch (operation) {
    case Operation::Distinct:return handleDistinct();
    case Operation::Sum:return handleSum();
    case Operation::Average:return handleAverage();
    default:
      throw IllegalStateException(
          "Internal error. DataWorkers::ResultAccumulator::step()");
  }
}

std::string DataWorkers::ResultAccumulator::getResult() {
  switch (operation) {
    case Operation::Distinct:return resultDistinct();
    case Operation::Sum:return resultSum();
    case Operation::Average:return resultAverage();
  }
  throw IllegalStateException("Internal error.");
}

void DataWorkers::ResultAccumulator::reset() {
  if (operation == Operation::Distinct) {
    distinct = false;
    return;
  }
  dataCount = 1;
}
std::string DataWorkers::ResultAccumulator::getResultForce() {
  switch (field->getFieldType()) {
    case ValueType::Integer: {
      return std::to_string(data._integer);
    }
    case ValueType::Double: {
      return std::to_string(data._double);
    }
    case ValueType::Currency: {
      return dec::toString(*data._currency);
    }
    case ValueType::String: {
      return data._string;
    }
    case ValueType::DateTime: {
      return data._dateTime->toString();
    }
    default:
      throw IllegalStateException(
          "Internal error. DataWorkers::ResultAccumulator::getResultForce()");
  }
}

std::string DataWorkers::ResultAccumulator::resultDistinct() {
  if (distinct) {
    return result;
  }
  return getResultForce();
}

std::string_view DataWorkers::ResultAccumulator::getName() {
  return field->getName();
}

DataContainer DataWorkers::ResultAccumulator::getContainer() {
  if (distinct) {
    return previousData;
  }
  return data;
}
