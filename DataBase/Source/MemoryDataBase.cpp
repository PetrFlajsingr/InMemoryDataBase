//
// Created by Petr Flajsingr on 2019-02-08.
//

#include <utility>
#include <MemoryDataBase.h>

std::string_view DataBase::MemoryDataBase::getName() const {
  return name;
}

void DataBase::MemoryDataBase::addTable(std::shared_ptr<DataSets::BaseDataSet> dataSet) {
  tables.emplace_back(Table{std::move(dataSet), {}});
}

void DataBase::MemoryDataBase::removeTable(std::string_view tableName) {
  const auto fncFindByName = [tableName]
      (const Table &table) {
    return table.dataSet->getName() == tableName;
  };

  if (auto it = std::find_if(tables.begin(),
                             tables.end(),
                             fncFindByName);
      it != tables.end()) {
    tables.erase(it);
  } else {
    throw DataBaseException();
  }
}

void DataBase::MemoryDataBase::addRelation(std::string_view relationName,
                                           DataBase::RelationType type,
                                           std::string_view table1Name,
                                           std::string_view table2Name) {

}

void DataBase::MemoryDataBase::cancelRelation(std::string_view relationName) {
  const auto fncFindByName = [relationName]
      (const Rel &relation) {
    return relation.name == relationName;
  };
  // TODO
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::MemoryDataBase::execSimpleQuery(
    std::string_view query,
    bool keepView,
    std::string_view viewName) {
  throw NotImplementedException();
//  auto view = std::make_shared<DataSets::MemoryViewDataSet>(viewName);

  // ... TODO: implement

  if (keepView) {

  }
  return nullptr;
}

std::shared_ptr<DataSets::BaseDataSet> DataBase::MemoryDataBase::execAggregateQuery(
    std::string_view query,
    std::string_view viewName) {
  throw NotImplementedException();
  auto view = std::make_shared<DataSets::MemoryDataSet>(viewName);

  // ... TODO: implement

  return view;
}
