//
// Created by Petr Flajsingr on 2019-02-12.
//

#include <SemanticAnalyser.h>

void DataBase::SemanticAnalyser::setInput(StructuredQuery &structuredQuery) {
  this->structuredQuery = structuredQuery;
}

void DataBase::SemanticAnalyser::analyse() {
  std::vector<std::string> tables;
  tables.emplace_back(structuredQuery.mainTable);
  for (const auto &val : structuredQuery.joins.data) {
    tables.emplace_back(val.joinedTable);
  }

  std::vector<std::string> aliases;
  for (const auto &val : structuredQuery.project.data) {
    if (!val.alias.empty()) {
      aliases.emplace_back(val.alias);
    }
  }
  for (const auto &val : structuredQuery.agr.data) {
    if (!val.field.alias.empty()) {
      aliases.emplace_back(val.field.alias);
    }
  }

  // check projection, agregation, selection, having, order tables
  for (const auto &val : structuredQuery.project.data) {
    if (std::find(tables.begin(), tables.end(), val.table) == tables.end()) {
      throw SemanticException(getErrorMsg(SemErrType::project, val.table));
    }
  }
  for (const auto &val : structuredQuery.agr.data) {
    if (std::find(tables.begin(), tables.end(), val.field.table)
        == tables.end()) {
      throw SemanticException("agr");
    }
  }
  for (const auto &val : structuredQuery.where.data) {
    if (std::find(tables.begin(), tables.end(), val.first.field.table)
        == tables.end() &&
        std::find(aliases.begin(), aliases.end(), val.first.field.alias)
            == aliases.end()) {
      throw SemanticException("where");
    }
  }
  for (const auto &val : structuredQuery.having.data) {
    if (std::find(tables.begin(), tables.end(), val.first.agreItem.field.table)
        == tables.end() &&
        std::find(aliases.begin(),
                  aliases.end(),
                  val.first.agreItem.field.alias) == aliases.end()) {
      throw SemanticException("having");
    }
  }
  for (const auto &val : structuredQuery.order.data) {
    if (std::find(tables.begin(), tables.end(), val.field.table) == tables.end()
        && std::find(aliases.begin(), aliases.end(), val.field.alias)
            == aliases.end()) {
      throw SemanticException("order");
    }
  }

  // check group by containing all non aggregated values
  if (!structuredQuery.agr.data.empty()) {
    for (const auto &val : structuredQuery.project.data) {
      auto found = std::find_if(structuredQuery.agr.data.begin(),
                                structuredQuery.agr.data.end(),
                                [&val](const AgreItem &item) {
                                  if (item.op == AgrOperator::group &&
                                      ((item.field.table == val.table
                                          && item.field.column == val.column)
                                          || item.field.alias == val.alias)) {
                                    return true;
                                  }
                                });
      if (found == structuredQuery.agr.data.end()) {
        throw SemanticException("group by");
      }
    }
  }

  // check join conditions
  std::vector<std::string> joinTables;
  joinTables.emplace_back(structuredQuery.mainTable);
  for (const auto &val : structuredQuery.joins.data) {
    if (std::find(joinTables.begin(), joinTables.end(), val.firstField.table)
        == joinTables.end()) {
      throw SemanticException("join main");
    }
    if (val.secondField.table != val.joinedTable) {
      throw SemanticException("join second");
    }
    joinTables.emplace_back(val.joinedTable);
  }
}

std::string DataBase::SemanticAnalyser::getErrorMsg(DataBase::SemanticAnalyser::SemErrType errType,
                                                    const std::string &value) {
  std::string result = "Semantic error: ";
  switch (errType) {
    case SemErrType::where:
      result += "Used of undeclared table \'" + value
          + "\' in WHERE clause";
      break;
    case SemErrType::joinMain:
      result += "Used of undeclared table \'" + value
          + "\' in WHERE clause";
      break;
    case SemErrType::joinSecond:break;
    case SemErrType::group:break;
    case SemErrType::order:break;
    case SemErrType::project:break;
    case SemErrType::having:break;
  }
  return result;
}
