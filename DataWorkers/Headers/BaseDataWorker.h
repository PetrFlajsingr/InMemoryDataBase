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

enum Operation {
  Distinct, Sum, Average
};

const std::string AggregationString[] {
  "",  "(Sum)", "(Avg)"
};

struct ProjectionOperation {
  std::string columnName;
  std::string tableName;
  Operation operation;
};

struct JoinOperation {
  std::string dataSet1Name;
  std::string column1Name;

  std::string dataSet2Name;
  std::string column2Name;
};

struct SelectionOperation {
  std::string dataSetName;
  std::string columnName;
  std::vector<std::string> reqs;
};

class BaseDataWorker {
 protected:
  std::vector<std::string> columnChoices;

  DataSets::BaseDataSet* dataset = nullptr;

  std::vector<ProjectionOperation> selectionOperations;

 public:
  BaseDataWorker() = default;

  virtual ~BaseDataWorker() {
    delete dataset;
  }

  virtual std::vector<std::string> getMultiChoiceNames() = 0;

  virtual std::vector<std::string> getChoices(std::string choiceName) = 0;

  virtual void filter(DataSets::FilterOptions &filters)=0;

  void setColumnOperations(std::vector<ProjectionOperation>& selectionOperations) {
    this->selectionOperations = selectionOperations;
  }

  virtual void writeResult(BaseDataWriter& writer) = 0;

  void setColumnChoices(std::vector<std::string> &data) {
    columnChoices = data;
  }

};
}  // namespace DataWorkers

#endif  //CSV_READER_BASEDATAWORKER_H
