//
// Created by Petr Flajsingr on 12/11/2018.
//

#include <MemoryDataSet.h>
#include "FINMDataWorker.h"

std::string DataWorkers::FINMDataWorker::getMultiChoiceNames() {
  throw NotImplementedException();
}

std::vector<std::string> DataWorkers::FINMDataWorker::getChoices(std::string choiceName) {
  throw NotImplementedException();
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
