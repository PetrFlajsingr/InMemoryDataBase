//
// Created by Petr Flajsingr on 2019-02-08.
//

#ifndef PROJECT_MEMORYDATABASE_H
#define PROJECT_MEMORYDATABASE_H

#include <AggregationMaker.h>
#include <BaseDataSet.h>
#include <Exceptions.h>
#include <LexicalAnalyser.h>
#include <MemoryDataSet.h>
#include <MemoryViewDataSet.h>
#include <QueryCommon.h>
#include <SemanticAnalyser.h>
#include <SyntaxAnalyser.h>
#include <mutex>
#include <string>
#include <vector>

namespace DataBase {

struct Table {
  std::shared_ptr<DataSets::MemoryDataSet> dataSet;
  std::string getName();
  std::mutex mutex;

  explicit Table(std::shared_ptr<DataSets::MemoryDataSet> dataSet);
};

struct View {
  std::shared_ptr<DataSets::MemoryViewDataSet> dataSet;
  std::string getName();
  std::mutex mutex;

  explicit View(std::shared_ptr<DataSets::MemoryViewDataSet> dataSet);
};
/**
 * Data base whose data are saved in memory.
 */
class MemoryDataBase {
public:
  explicit MemoryDataBase(std::string name);

  void addTable(const std::shared_ptr<DataSets::MemoryDataSet> &dataSet);
  void removeTable(std::string_view tableName);
  void removeView(std::string_view viewName);

  [[nodiscard]] std::shared_ptr<Table> tableByName(std::string_view tableName) const;
  [[nodiscard]] std::shared_ptr<View> viewByName(std::string_view viewName) const;
  /**
   * Execute a query not containing aggregation.
   * @param query query to execute
   * @param keepView save result in database
   * @param viewName name of the result
   * @return View to the queried data
   */
  std::shared_ptr<View> execSimpleQuery(std::string_view query, bool keepView, std::string_view viewName);
  /**
   * Execute a query containing aggregation.
   * @param query query to execute
   * @param viewName name of the result
   * @return View to the queried data
   */
  std::shared_ptr<View> execAggregateQuery(std::string_view query, std::string_view viewName);

  [[nodiscard]] std::string getName() const;

  [[nodiscard]] const std::vector<std::shared_ptr<Table>> &getTables() const;

private:
  std::vector<std::shared_ptr<Table>> tables;
  std::vector<std::shared_ptr<View>> views;

  StructuredQuery validateQuery(StructuredQuery &query) const;

  StructuredQuery parseQuery(std::string_view query);

  std::string name;

  LexicalAnalyser lexicalAnalyser;
  SyntaxAnalyser syntaxAnalyser;
  SemanticAnalyser semanticAnalyser;

  // operations on views
  std::shared_ptr<View> doJoin(const StructuredQuery &query);
  // TODO: implement and/or
  std::shared_ptr<View> doWhere(const StructuredQuery &query, std::shared_ptr<View> &view);
  std::shared_ptr<View> doOrder(const StructuredQuery &query, std::shared_ptr<View> &view);
  std::shared_ptr<View> doProject(const StructuredQuery &query, std::shared_ptr<View> &view);
  std::shared_ptr<View> doHaving(const StructuredQuery &query, std::shared_ptr<View> &view);
};

} // namespace DataBase

#endif // PROJECT_MEMORYDATABASE_H
