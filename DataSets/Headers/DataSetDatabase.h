//
// Created by Petr Flajsingr on 2019-02-03.
//

#ifndef DATASETS_DATASETMANAGER_H
#define DATASETS_DATASETMANAGER_H

#include <gsl/gsl>
#include <string>
#include <vector>
#include <BaseDataSet.h>

namespace DataSets {

class DataSetDatabase {
 public:
  explicit DataSetDatabase(const std::string &name);

  void add(std::shared_ptr<DataSets::BaseDataSet> dataSet);

  void remove(std::string_view name);

  std::shared_ptr<DataSets::BaseDataSet> get(std::string_view name) const;

  std::string_view getName() const;

 private:
  std::vector<std::shared_ptr<DataSets::BaseDataSet>> dataSets;

  std::string name;
};
}  // namespace DataSets


#endif //DATASETS_DATASETMANAGER_H
