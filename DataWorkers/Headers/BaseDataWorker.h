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

class BaseDataWorker {
 protected:
  std::vector<std::string> columnChoices;

  DataSets::BaseDataSet* dataset = nullptr;

  QueryData queryData;

  std::vector<DataSets::BaseDataSet*> additionalDataSets;

 public:
  virtual ~BaseDataWorker() = default;

  virtual std::vector<std::string> getMultiChoiceNames() = 0;

  virtual std::vector<std::string> getChoices(std::string choiceName) = 0;

  virtual void writeResult(DataWriters::BaseDataWriter &writer,
                           const std::string &sql) = 0;

  void setColumnChoices(std::vector<std::string> &data) {
    columnChoices = data;
  }

  void addDataSet(gsl::not_null<DataSets::BaseDataSet *> dataset) {
    additionalDataSets.push_back(dataset);
  }

  void clearDataSets() {
    additionalDataSets.clear();
  }
};
}  // namespace DataWorkers

#endif  //DATAWORKERS_HEADERS_BASEDATAWORKER_H_
