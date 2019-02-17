//
// Created by Petr Flajsingr on 2019-02-08.
//

#include <utility>
#include <map>
#include <MemoryDataBase.h>
#include <QueryException.h>

DataBase::MemoryDataBase::MemoryDataBase(const std::string &name)
    : name(name) {}

std::string_view DataBase::MemoryDataBase::getName() const {
  return name;
}

void DataBase::MemoryDataBase::addTable(std::shared_ptr<DataSets::MemoryDataSet> dataSet) {
  tables.emplace_back(dataSet);
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
    throw DataBaseException("Table " + std::string(tableName) + " not found.");
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

std::shared_ptr<DataBase::View> DataBase::MemoryDataBase::execSimpleQuery(
    std::string_view query,
    bool keepView,
    std::string_view viewName) {
  lexicalAnalyser.setInput(std::string(query));
  syntaxAnalyser.setInput(lexicalAnalyser.getAllTokens());
  auto strQuery = syntaxAnalyser.analyse();
  semanticAnalyser.setInput(strQuery);
  auto semQuery = semanticAnalyser.analyse();
  validateQuery(semQuery);

  if (!semQuery.agr.data.empty()) {
    throw DataBaseException("Provided query is not \"simple\", "
                            "use DataBase::MemoryDataBase::execAggregateQuery "
                            "for queries using aggregation");
  }

  /*auto resultView = std::make_shared<DataSets::MemoryViewDataSet>(viewName);
  if (keepView) {
    views.emplace_back(resultView);
  }


  return resultView;*/
  return std::make_shared<View>(nullptr);
}

std::shared_ptr<DataSets::BaseDataSet> DataBase::MemoryDataBase::execAggregateQuery(
    std::string_view query,
    std::string_view viewName) {
  lexicalAnalyser.setInput(std::string(query));
  syntaxAnalyser.setInput(lexicalAnalyser.getAllTokens());
  auto strQuery = syntaxAnalyser.analyse();
  semanticAnalyser.setInput(strQuery);
  auto semQuery = semanticAnalyser.analyse();
  validateQuery(semQuery);

  if (semQuery.agr.data.empty()) {
    throw DataBaseException("Provided query is \"simple\", "
                            "use DataBase::MemoryDataBase::execSimpleQuery "
                            "for queries not using aggregation");
  }

  return nullptr;
}

void DataBase::MemoryDataBase::validateQuery(DataBase::StructuredQuery query) const {
  std::vector<std::pair<std::string, bool>> tables;
  tables.emplace_back(query.mainTable, false);

  std::vector<std::string> savedTables;
  std::transform(this->tables.begin(),
                 this->tables.end(),
                 std::back_inserter(savedTables),
                 [](const Table &table) {
                   return table.dataSet->getName();
                 });

  for (auto &val : tables) {
    val.second = std::find(savedTables.begin(), savedTables.end(), val.first)
        != savedTables.end();
  }

  for (const auto &val : query.joins.data) {
    tables.emplace_back(val.joinedTable, false);
  }
  std::string errMsg;
  for (auto &val : tables) {
    val.second = std::find(savedTables.begin(), savedTables.end(), val.first)
        != savedTables.end();
    if (!val.second) {
      errMsg += "Unknown table: " + val.first + "\n";
    }
  }
  if (!errMsg.empty()) {
    throw DataBaseQueryException("DataBase exception: " + errMsg);
  }

  std::vector<std::pair<FieldId, bool>> fields;
  for (const auto &val : query.project.data) {
    fields.emplace_back(val, false);
  }

  for (const auto &val : query.joins.data) {
    fields.emplace_back(val.firstField, false);
    fields.emplace_back(val.secondField, false);
  }
  for (const auto &val : query.having.data) {
    fields.emplace_back(val.first.agreItem.field, false);
  }
  for (const auto &val : query.agr.data) {
    fields.emplace_back(val.field, false);
  }
  for (const auto &val : query.order.data) {
    fields.emplace_back(val.field, false);
  }
  for (const auto &val : query.where.data) {
    fields.emplace_back(val.first.field, false);
  }

  std::map<std::string, std::vector<std::string>> savedFields;
  for (const auto &table : this->tables) {
    savedFields[table.dataSet->getName()] = table.dataSet->getFieldNames();
  }

  for (auto &val : fields) {
    val.second = std::find(savedFields[val.first.table].begin(),
                           savedFields[val.first.table].end(), val.first.column)
        != savedFields[val.first.table].end();
    if (!val.second && !val.first.column.empty()) {
      errMsg +=
          "Unknown field: " + val.first.column + " in table: " + val.first.table
              + "\n";
    }
  }
  if (!errMsg.empty()) {
    throw DataBaseQueryException("DataBase exception: " + errMsg);
  }

  for (const auto &val : query.joins.data) {
    auto field1Type =
        tableByName(val.firstField.table).dataSet->fieldByName(val.firstField.column)->getFieldType();
    auto field2Type =
        tableByName(val.secondField.table).dataSet->fieldByName(val.secondField.column)->getFieldType();
    if (field1Type != field2Type) {
      errMsg = "Field " + val.firstField.table + "." + val.firstField.column
          + " has different data type than field" + val.secondField.table + "."
          + val.secondField.column;
      throw DataBaseQueryException("DataBase exception: " + errMsg);
    }
  }
}

const DataBase::Table &DataBase::MemoryDataBase::tableByName(std::string_view tableName) const {
  for (auto &table : tables) {
    if (table.dataSet->getName() == tableName) {
      return table;
    }
  }
  std::string errMsg = "Table named \"" + std::string(name)
      + "\" not found. DataBase::MemoryDataBase::tableByName";
  throw InvalidArgumentException(errMsg.c_str());
}

DataBase::Table::Table(const std::shared_ptr<DataSets::MemoryDataSet> &dataSet)
    : dataSet(dataSet) {}
DataBase::View::View(const std::shared_ptr<DataSets::MemoryViewDataSet> &dataSet)
    : dataSet(dataSet) {}
