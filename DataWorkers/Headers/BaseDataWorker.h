//
// Created by Petr Flajsingr on 12/11/2018.
//

#ifndef DATAWORKERS_HEADERS_BASEDATAWORKER_H_
#define DATAWORKERS_HEADERS_BASEDATAWORKER_H_

#include <string>
#include <CsvWriter.h>
#include <BaseDataSet.h>
#include <FilterStructures.h>
#include "SQLParser.h"

namespace DataWorkers {

const std::string AggregationString[] {
  "",  "(Sum)", "(Avg)"
};

class BaseDataWorker {
 protected:
  std::vector<std::string> columnChoices;

  DataSets::BaseDataSet* dataset = nullptr;

  QueryData queryData;

  std::vector<DataSets::BaseDataSet*> additionalDataSets;

 public:
  BaseDataWorker() = default;

  virtual ~BaseDataWorker() {
    delete dataset;
  }

  virtual std::vector<std::string> getMultiChoiceNames() = 0;

  virtual std::vector<std::string> getChoices(std::string choiceName) = 0;

  virtual void writeResult(BaseDataWriter& writer,
      std::string &sql) = 0;

  void setColumnChoices(std::vector<std::string> &data) {
    columnChoices = data;
  }

  void addDataSet(DataSets::BaseDataSet* dataset) {
    additionalDataSets.push_back(dataset);
  }

  void clearDataSets() {
    additionalDataSets.clear();
  }
};
}  // namespace DataWorkers

#endif  //DATAWORKERS_HEADERS_BASEDATAWORKER_H_
