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

struct ColumnOperation {
  std::string columnName;
  Operation operation;
};

class BaseDataWorker {
 protected:
  std::vector<std::string> columnChoices;

  DataSets::BaseDataSet* dataset = nullptr;

  std::vector<ColumnOperation> columnOperations;

 public:
  BaseDataWorker() = default;

  virtual ~BaseDataWorker() {
    delete dataset;
  }

  virtual std::vector<std::string> getMultiChoiceNames() = 0;

  virtual std::vector<std::string> getChoices(std::string choiceName) = 0;

  virtual void filter(DataSets::FilterOptions &filters)=0;

  void setColumnOperations(std::vector<ColumnOperation>& columnOperations) {
    this->columnOperations = columnOperations;
  }

  virtual void writeResult(CsvWriter& writer) = 0;

  void setColumnChoices(std::vector<std::string> &data) {
    columnChoices = data;
  }

};
}  // namespace DataWorkers

#endif  //CSV_READER_BASEDATAWORKER_H
