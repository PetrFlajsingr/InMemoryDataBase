//
// Created by Petr Flajsingr on 2019-02-08.
//

#ifndef PROJECT_MEMORYDATABASE_H
#define PROJECT_MEMORYDATABASE_H

#include <vector>
#include <string>
#include <BaseDataSet.h>
#include <Relation.h>
#include <MemoryViewDataSet.h>

namespace DataBase {
class DataBaseException : public std::exception {

};

class Rel;

struct Table {
  std::shared_ptr<DataSets::BaseDataSet> dataSet;
  std::vector<Rel> relations;
};

struct View {
  Table *table;
  std::shared_ptr<DataSets::MemoryViewDataSet> dataSet;
  std::vector<Rel> relations;
};

class Rel {
 public:
  Table secondary;
  std::shared_ptr<DataSets::BaseField> primaryField;
  std::shared_ptr<DataSets::BaseField> secondaryField;

  std::string name;

  std::vector<std::pair<DataSets::DataSetRow *, DataSets::DataSetRow *>>
      connectedData;

  void rebuildConnections();
};

class MemoryDataBase {
 public:

  void addTable(std::shared_ptr<DataSets::BaseDataSet> dataSet);

  void removeTable(std::string_view tableName);

  void addRelation(std::string_view relationName,
                   RelationType type,
                   std::string_view table1Name,
                   std::string_view table2Name);

  void cancelRelation(std::string_view relationName);

  std::shared_ptr<DataSets::MemoryViewDataSet> execSimpleQuery(
      std::string_view query,
      bool keepView,
      std::string_view viewName);

  std::shared_ptr<DataSets::BaseDataSet> execAggregateQuery(
      std::string_view query,
      std::string_view viewName);

  std::string_view getName() const;

 private:
  std::vector<Table> tables;
  std::vector<std::shared_ptr<View>> views;

  std::string name;

};

}  // namespace DataBase

#endif //PROJECT_MEMORYDATABASE_H
