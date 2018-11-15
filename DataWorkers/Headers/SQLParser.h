#include <utility>

//
// Created by Petr Flajsingr on 15/11/2018.
//

#ifndef CSV_READER_SQLPARSER_H
#define CSV_READER_SQLPARSER_H

#include <string>
#include <Utilities.h>
#include <iostream>
#include <algorithm>
#include "BaseDataWorker.h"

namespace DataWorkers {
  class SQLException : public std::exception {};

  struct QueryData {
    std::vector<ProjectionOperation> projections;
    std::vector<SelectionOperation> selections;
    std::vector<JoinOperation> joins;
    std::string mainTable;
  };

  class SQLParser {
   private:

   public:
    static QueryData parse(std::string sql) {
      auto splitSql = Utilities::splitStringByDelimiter(std::move(sql), " ");

      QueryData result;

      int iter = 0;
      if (splitSql[iter] != "SELECT") {
        throw SQLException();
      }
      iter++;

      // projekce
      while (splitSql[iter] != "FROM") {
        if(Utilities::endsWith(splitSql[iter], ",")) {
          splitSql[iter] = splitSql[iter].substr(0, splitSql[iter].size() - 1);
        }

        auto colInfo = Utilities::splitStringByDelimiter(splitSql[iter], ".");
        result.projections.push_back({colInfo[1], colInfo[0], Operation::Distinct});

        iter++;
      }
      iter++;
      result.mainTable = splitSql[iter];

      iter++;

      // join



      while (splitSql[iter] != "WHERE") {
        if (splitSql[iter] != "JOIN") {
          throw SQLException();
        }
        iter++;

        JoinOperation op;
        op.table2Name = splitSql[iter];

        iter++;

        if (splitSql[iter] != "ON") {
          throw SQLException();
        }
        iter++;

        auto colInfo = Utilities::splitStringByDelimiter(splitSql[iter], ".");
        op.table1Name = colInfo[0];
        op.column1Name = colInfo[1];
        iter++;

        if (splitSql[iter] != "=") {
          throw SQLException();
        }
        iter++;

        colInfo = Utilities::splitStringByDelimiter(splitSql[iter], ".");
        op.column2Name = colInfo[1];

        result.joins.push_back(op);
        iter++;
      }

      // selekce
      while (iter < splitSql.size()) {
        SelectionOperation selectionOperation;
        iter++;

        auto colInfo = Utilities::splitStringByDelimiter(splitSql[iter], ".");
        selectionOperation.columnName = colInfo[1];
        selectionOperation.tableName = colInfo[0];
        iter++;
        if (splitSql[iter] != "=") {
          throw SQLException();
        }

        do {
          iter++;
          selectionOperation.reqs.push_back(splitSql[iter]);
          iter++;
        } while (iter < splitSql.size() && splitSql[iter] == "|");
        if (iter < splitSql.size() && splitSql[iter] != "AND") {
          throw SQLException();
        }

        result.selections.push_back(selectionOperation);
      }

      return result;
   }
  };
}  // namespace DataWorkers

#endif //CSV_READER_SQLPARSER_H
