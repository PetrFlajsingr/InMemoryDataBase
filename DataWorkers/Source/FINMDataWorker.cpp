//
// Created by Petr Flajsingr on 12/11/2018.
//

#include <MemoryDataSet.h>
#include <FINMDataWorker.h>

#include "FINMDataWorker.h"

std::vector<std::string> DataWorkers::FINMDataWorker::getMultiChoiceNames() {
  return columnChoices;
}

std::vector<std::string> DataWorkers::FINMDataWorker::getChoices(std::string choiceName) {
  DataSets::BaseField* field = dataset->fieldByName(choiceName);

  if (field == nullptr) {
    return {};
  }

  auto * memoryDataSet = dynamic_cast<DataSets::MemoryDataSet*>(dataset);

  memoryDataSet->sort(field->getIndex(), SortOrder::ASCENDING);

  std::string value;

  std::vector<std::string> result;

  while (!memoryDataSet->eof()) {
    if (field->getAsString() != value) {
      value = field->getAsString();
      result.push_back(value);
    }
    memoryDataSet->next();
  }

  return result;
}

void DataWorkers::FINMDataWorker::filter(DataSets::FilterOptions &filters) {
  dynamic_cast<DataSets::MemoryDataSet*>(dataset)->filter(filters);
}

DataWorkers::FINMDataWorker::FINMDataWorker(
    DataProviders::BaseDataProvider *dataProvider,
    std::vector<ValueType> fieldTypes)
    : BaseDataWorker() {
  dataset = new DataSets::MemoryDataSet();

  dataset->setDataProvider(dataProvider);

  dataset->setFieldTypes(fieldTypes);

  dataset->open();
}

void DataWorkers::FINMDataWorker::writeResult(BaseDataWriter &writer) {
  writeHeaders(writer);

  auto *memoryDataSet = dynamic_cast<DataSets::MemoryDataSet*>(dataset);

  memoryDataSet->sort(memoryDataSet
    ->fieldByName(columnOperations[0].columnName)->getIndex(),
    ASCENDING);

  std::vector<ResultAccumulator*> accumulators;

  for (auto &op : columnOperations) {
    accumulators.push_back(
        new ResultAccumulator(memoryDataSet->fieldByName(op.columnName),
        op.operation));
  }

  bool savedResults = false;
  std::vector<std::string> results;
  while (!memoryDataSet->eof()) {
    for (auto acc : accumulators) {
      if (acc->step() && !savedResults) {
        for (auto toSave : accumulators) {
          results.push_back(toSave->getResult());

          toSave->reset();
        }
        savedResults = true;
      }
    }

    if (savedResults) {
      writer.writeRecord(results);
      results.clear();

      savedResults = false;
    }

    memoryDataSet->next();
  }

  for (auto toSave : accumulators) {
    results.push_back(toSave->getResultForce());

    writer.writeRecord(results);
  }

  for (auto *acc : accumulators) {
    delete acc;
  }
}

void DataWorkers::FINMDataWorker::writeHeaders(BaseDataWriter &writer) {
  std::vector<std::string> header;

  std::transform(columnOperations.begin(),
                 columnOperations.end(),
                 std::back_inserter(header),
                 [](const ColumnOperation &op) {
    return op.columnName;
  });

  writer.writeHeader(header);
}




DataWorkers::ResultAccumulator::ResultAccumulator(DataSets::BaseField *field,
    Operation op) : field(field), operation(op) {
  switch (field->getFieldType()) {
    case STRING_VAL:
      data._string = nullptr;
      break;
    case INTEGER_VAL:
      data._int = 0;
      break;
    case DOUBLE_VAL:
      data._double = 0.0;
      break;
    case CURRENCY:
      data._currency = new Currency();
      break;
  }
}

bool DataWorkers::ResultAccumulator::handleDistinct() {
  switch (field->getFieldType()) {
    case INTEGER_VAL: {
      int value = reinterpret_cast<DataSets::IntegerField *>(field)
          ->getAsInteger();
      if (value != data._int) {
        result = std::to_string(data._int);
        data._int = value;
        return true;
      }
      break;
    }
    case DOUBLE_VAL: {
      double value = reinterpret_cast<DataSets::DoubleField *>(field)
          ->getAsDouble();
      if (value != data._double) {
        result = std::to_string(data._double);
        data._double = value;
        return true;
      }
      break;
    }
    case STRING_VAL: {
      std::string value = field->getAsString();
      if (data._string == nullptr) {
        data._string = strdup(value.c_str());
        return false;
      }
      if (value != data._string) {
        result = data._string;
        delete[] data._string;
        data._string = strdup(value.c_str());
        return true;
      }
      break;
    }
    case CURRENCY: {
      Currency value = reinterpret_cast<DataSets::CurrencyField *>(field)
          ->getAsCurrency();
      if (value != *data._currency) {
        result = dec::toString(*data._currency);
        *data._currency = value;
        return true;
      }
      break;
    }
  }
  return false;
}

bool DataWorkers::ResultAccumulator::handleSum() {
  switch (field->getFieldType()) {
    case INTEGER_VAL: {
      int value = reinterpret_cast<DataSets::IntegerField *>(field)
          ->getAsInteger();
      data._int += value;
      break;
    }
    case DOUBLE_VAL: {
      double value = reinterpret_cast<DataSets::DoubleField *>(field)
          ->getAsDouble();
      data._double += value;
      break;
    }
    case STRING_VAL: {
      throw IllegalStateException("Internal error.");
    }
    case CURRENCY: {
      Currency value = reinterpret_cast<DataSets::CurrencyField *>(field)
          ->getAsCurrency();
      *data._currency += value;
      break;
    }
  }
  return false;
}

bool DataWorkers::ResultAccumulator::handleAverage() {
  dataCount++;
  switch (field->getFieldType()) {
    case INTEGER_VAL: {
      int value = reinterpret_cast<DataSets::IntegerField *>(field)
          ->getAsInteger();
      data._int += value;
      break;
    }
    case DOUBLE_VAL: {
      double value = reinterpret_cast<DataSets::DoubleField *>(field)
          ->getAsDouble();
      data._double += value;
      break;
    }
    case STRING_VAL: {
      throw IllegalStateException("Internal error.");
    }
    case CURRENCY: {
      Currency value = reinterpret_cast<DataSets::CurrencyField *>(field)
          ->getAsCurrency();
      *data._currency += value;
      break;
    }
  }
  return false;
}

std::string DataWorkers::ResultAccumulator::resultSum() {
  switch (field->getFieldType()) {
    case INTEGER_VAL: {
      return std::to_string(data._int);
    }
    case DOUBLE_VAL: {
      return std::to_string(data._double);
    }
    case CURRENCY: {
      return dec::toString(*data._currency);
    }
  }
}

std::string DataWorkers::ResultAccumulator::resultAverage() {
  switch (field->getFieldType()) {
    case INTEGER_VAL: {
      return std::to_string(data._int / dataCount);
    }
    case DOUBLE_VAL: {
      return std::to_string(data._double / dataCount);
    }
    case CURRENCY: {
      return dec::toString(*data._currency / Currency(dataCount));
    }
  }
}

bool DataWorkers::ResultAccumulator::step() {
  switch (operation) {
    case Distinct:
      return handleDistinct();
    case Sum:
      return handleSum();
    case Average:
      return handleAverage();
    default:
      throw IllegalStateException("Internal error.");
  }
}

std::string DataWorkers::ResultAccumulator::getResult() {
  switch (operation) {
    case Distinct:
      return result;
    case Sum:
      return resultSum();
    case Average:
      return resultAverage();
  }
  throw IllegalStateException("Internal error.");
}

void DataWorkers::ResultAccumulator::reset() {
  if(operation == Distinct) {
    return;
  }
  dataCount = 0;

  switch (field->getFieldType()) {
    case INTEGER_VAL:
      data._int = 0;
      break;
    case DOUBLE_VAL:
      data._double = 0;
      break;
    case CURRENCY:
      *data._currency = 0;
      break;
  }
}
std::string DataWorkers::ResultAccumulator::getResultForce() {
  switch (field->getFieldType()) {
    case INTEGER_VAL: {
      return std::to_string(data._int);
    }
    case DOUBLE_VAL: {
      return std::to_string(data._double);
    }
    case CURRENCY: {
      return dec::toString(*data._currency);
    }
    case STRING_VAL: {
      return data._string;
    }
  }
}
