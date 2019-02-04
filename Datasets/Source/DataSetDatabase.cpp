//
// Created by Petr Flajsingr on 2019-02-03.
//

#include <DataSetDatabase.h>

DataSets::DataSetDatabase::DataSetDatabase(const std::string &name)
    : name(name) {}

void DataSets::DataSetDatabase::add(std::shared_ptr<DataSets::BaseDataSet> dataSet) {
  dataSets.emplace_back(dataSet);
}

void DataSets::DataSetDatabase::remove(std::string_view name) {
  const auto findByName =
      [name](const std::shared_ptr<DataSets::BaseDataSet> &dataSet) {
        return dataSet->getName() == name;
      };
  if (const auto
        it = std::find_if(dataSets.begin(), dataSets.end(), findByName); it
      != dataSets.end()) {
    dataSets.erase(it);
  }
}

std::shared_ptr<DataSets::BaseDataSet> DataSets::DataSetDatabase::get(std::string_view name) const {
  const auto findByName =
      [name](const std::shared_ptr<DataSets::BaseDataSet> dataSet) {
        return dataSet->getName() == name;
      };
  if (const auto
        it = std::find_if(dataSets.begin(), dataSets.end(), findByName); it
      != dataSets.end()) {
    return *it;
  }
}

std::string_view DataSets::DataSetDatabase::getName() const {
  return name;
}
