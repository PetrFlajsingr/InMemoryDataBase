//
// Created by Petr Flajsingr on 12/11/2018.
//

#include <MemoryDataSet.h>
#include "FINMDataWorker.h"

std::vector<std::string> DataWorkers::FINMDataWorker::getMultiChoiceNames() {
  return columnChoices;
}

std::vector<std::string> DataWorkers::FINMDataWorker::getChoices(std::string choiceName) {
  DataSets::BaseField* field = dataset->fieldByName(choiceName);

  if(field == nullptr) {
    return {};
  }

  DataSets::MemoryDataSet* memoryDataSet = dynamic_cast<DataSets::MemoryDataSet*>(dataset);

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

void DataWorkers::FINMDataWorker::setFilters(std::vector<DataSets::FilterOptions> filters) {
  throw NotImplementedException();
}

DataWorkers::FINMDataWorker::FINMDataWorker(CsvWriter *writer,
    DataProviders::BaseDataProvider *dataProvider,
    std::vector<ValueType> fieldTypes)
    : BaseDataWorker(writer, dataProvider) {
  dataset = new DataSets::MemoryDataSet();

  dataset->setDataProvider(dataProvider);

  dataset->setFieldTypes(fieldTypes);

  dataset->open();
}
