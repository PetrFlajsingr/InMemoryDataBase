//
// Created by Petr Flajsingr on 2019-01-25.
//

#ifndef CSV_READER_DATASETMERGER_H
#define CSV_READER_DATASETMERGER_H

#include <vector>
#include <string>
#include <BaseDataSet.h>
#include <MemoryDataSet.h>

namespace DataWorkers {

class DataSetMerger {
 private:
  std::vector<DataSets::BaseDataSet *> dataSets;

  void appendData(std::vector<DataSets::BaseField *> fields, DataSets::BaseDataSet *dataset);
 public:
  DataSetMerger();

  void addDataSet(gsl::not_null<DataSets::BaseDataSet *> dataSet) {
    dataSets.emplace_back(dataSet);
  }

  void removeDataSet(const std::string &dataSetName);

  DataSets::BaseDataSet *mergeDataSets(const std::string &dataSetName1,
                                       const std::string &dataSetName2,
                                       const std::string &columnName1,
                                       const std::string &columnName2);

};

}  //namespace DataWorkers

#endif //CSV_READER_DATASETMERGER_H
