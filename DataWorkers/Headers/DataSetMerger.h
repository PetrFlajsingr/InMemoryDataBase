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
 public:
  DataSetMerger();

  void addDataSet(gsl::not_null<DataSets::BaseDataSet *> dataSet) {
    dataSets.emplace_back(dataSet);
  }

  void removeDataSet(std::string_view dataSetName);

  gsl::not_null<DataSets::BaseDataSet *> mergeDataSets(std::string_view dataSetName1,
                                                       std::string_view dataSetName2,
                                                       std::string_view columnName1,
                                                       std::string_view columnName2);

 private:
  std::vector<DataSets::BaseDataSet *> dataSets;

  void appendData(std::vector<DataSets::BaseField *> fields,
                  DataSets::BaseDataSet *dataset);
};

}  //namespace DataWorkers

#endif //CSV_READER_DATASETMERGER_H
