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
#include <Exceptions.h>
#include <QueryCommon.h>
#include <LexicalAnalyser.h>
#include <SyntaxAnalyser.h>
#include <SemanticAnalyser.h>
#include <MemoryDataSet.h>

namespace DataBase {
class Rel;

struct Table {
  std::shared_ptr<DataSets::MemoryDataSet> dataSet;
  std::vector<Rel> relations;

  explicit Table(const std::shared_ptr<DataSets::MemoryDataSet> &dataSet);
};

struct View {
  std::shared_ptr<DataSets::MemoryViewDataSet> dataSet;
  std::vector<Rel> relations;

  explicit View(const std::shared_ptr<DataSets::MemoryViewDataSet> &dataSet);
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
  explicit MemoryDataBase(const std::string &name);

  void addTable(std::shared_ptr<DataSets::MemoryDataSet> dataSet);

  void removeTable(std::string_view tableName);

  const Table &tableByName(std::string_view tableName) const;

  void addRelation(std::string_view relationName,
                   RelationType type,
                   std::string_view table1Name,
                   std::string_view table2Name);

  void cancelRelation(std::string_view relationName);

  std::shared_ptr<View> execSimpleQuery(
      std::string_view query,
      bool keepView,
      std::string_view viewName);

  std::shared_ptr<DataSets::BaseDataSet> execAggregateQuery(
      std::string_view query,
      std::string_view viewName);

  std::string_view getName() const;

  void validateQuery(StructuredQuery query) const;

 private:
  std::vector<Table> tables;
  std::vector<std::shared_ptr<View>> views;

  std::string name;

  LexicalAnalyser lexicalAnalyser;
  SyntaxAnalyser syntaxAnalyser;
  SemanticAnalyser semanticAnalyser;
};

}  // namespace DataBase

#endif //PROJECT_MEMORYDATABASE_H
