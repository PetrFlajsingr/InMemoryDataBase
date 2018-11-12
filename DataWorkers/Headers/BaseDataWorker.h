//
// Created by Petr Flajsingr on 12/11/2018.
//

#ifndef CSV_READER_BASEDATAWORKER_H
#define CSV_READER_BASEDATAWORKER_H

#include <string>
#include <CsvWriter.h>
#include <BaseDataSet.h>
#include <FilterStructures.h>

namespace DataWorkers {
class BaseDataWorker {
 protected:
  CsvWriter* writer = nullptr;

  DataSets::BaseDataSet* dataset = nullptr;
 public:
  BaseDataWorker(CsvWriter *writer, DataProviders::BaseDataProvider* dataProvider) : writer(writer) {}

  virtual ~BaseDataWorker() {
    delete writer;
    delete dataset;
  }

  virtual std::vector<std::string> getMultiChoiceNames() = 0;

  virtual std::vector<std::string> getChoices(std::string choiceName) = 0;

  void setDataWriter(CsvWriter& writer) {
    this->writer = &writer;
  }

  virtual void setFilters(std::vector<DataSets::FilterOptions> filters)=0;
};
} // namespace DataWorkers

#endif  //CSV_READER_BASEDATAWORKER_H
