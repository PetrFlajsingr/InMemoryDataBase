//
// Created by Petr Flajsingr on 15/11/2018.
//

#ifndef CSV_READER_SQLPARSER_H
#define CSV_READER_SQLPARSER_H

#include <string>
#include <Utilities.h>
#include <iostream>
#include <algorithm>

namespace DataWorkers {
  class SQLException : public std::exception {};

  enum Operation {
    Distinct, Sum, Average
  };

  struct ProjectionOperation {
    std::string columnName;
    std::string tableName;
    Operation operation;
  };

enum JoinType {
  LeftJoin, NormalJoin
};

  struct JoinOperation {
    JoinType joinType;
    std::string table1Name;
    std::string column1Name;

    std::string table2Name;
    std::string column2Name;
  };

  struct SelectionOperation {
    std::string tableName;
    std::string columnName;
    std::vector<std::string> reqs;
  };

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

        if (splitSql[iter].find("(") != std::string::npos) {
          auto split = Utilities::splitStringByDelimiter(splitSql[iter], "(");
          Operation operation;
          if (split[0] == "AVG") {
            operation = Average;
          } else if (split[0] == "SUM") {
            operation = Sum;
          } else {
            throw SQLException();
          }

          split[1] = split[1].substr(0, split[1].size() - 1);
          auto colInfo = Utilities::splitStringByDelimiter(split[1], ".");

          result.projections.push_back({colInfo[1], colInfo[0], operation});
        } else {
          auto colInfo = Utilities::splitStringByDelimiter(splitSql[iter], ".");
          result.projections.push_back({colInfo[1], colInfo[0], Operation::Distinct});
        }



        iter++;
      }
      iter++;
      result.mainTable = splitSql[iter];

      iter++;

      // join


      if(splitSql.size() == iter) {
        return result;
      }

      while (iter < splitSql.size() && splitSql[iter] != "WHERE") {
        JoinOperation op;
        if (splitSql[iter] != "JOIN") {
          if (splitSql[iter] != "LEFT") {
            throw SQLException();
          }
          op.joinType = LeftJoin;
          iter++;
          if (splitSql[iter] != "JOIN") {
            throw SQLException();
          }
        } else {
          op.joinType = NormalJoin;
        }
        iter++;

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
