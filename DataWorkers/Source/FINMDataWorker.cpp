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

void DataWorkers::FINMDataWorker::writeResult(CsvWriter &writer) {
  writeHeaders(writer);


}

void DataWorkers::FINMDataWorker::writeHeaders(CsvWriter &writer) {
  std::vector<std::string> header;

  std::transform(columnOperations.begin(),
                 columnOperations.end(),
                 std::back_inserter(header),
                 [](const ColumnOperation &op) {
    return op.columnName;
  });

  writer.writeHeader(header);
}
