//
// Created by Petr Flajsingr on 2019-02-08.
//

#include <catch2/catch.hpp>
#include <SQLParser.h>

TEST_CASE("Typical parse", "[SQLParser]") {
  DataWorkers::QueryData expectedOutput;
  expectedOutput.projections.push_back({"uganda", "table1",
                                        Operation::Distinct});
  expectedOutput.projections.push_back({"letadlo", "table1",
                                        Operation::Distinct});
  expectedOutput.projections.push_back({"krava", "table2",
                                        Operation::Distinct});
  expectedOutput.projections.push_back({"id", "table3", Operation::Distinct});
  expectedOutput.joins.push_back({DataWorkers::JoinType::LeftJoin, "table1",
                                  "uganda", "table2", "id"});
  expectedOutput.joins.push_back({DataWorkers::JoinType::LeftJoin, "table2",
                                  "id", "table3", "id"});
  expectedOutput.selections.push_back({"table1", "uganda", {"10", "5", "8"}});
  expectedOutput.selections.push_back({"table1", "letadlo", {"22", "23"}});

  DataWorkers::QueryData data = DataWorkers::SQLParser::parse(
      "SELECT table1.uganda, table1.letadlo, table2.krava, table3.id FROM table1 JOIN table2 ON table1.uganda = table2.id JOIN table3 ON table2.id = table3.id WHERE table1.uganda = 10 | 5 | 8 AND table1.letadlo = 22 | 23");

  REQUIRE(data.selections.size() == expectedOutput.selections.size());
  REQUIRE(data.projections.size() == expectedOutput.projections.size());
  REQUIRE(data.joins.size() == expectedOutput.joins.size());

  for (gsl::index i = 0; i < data.selections.size(); ++i) {
    CHECK(data.selections[i].tableName ==
        expectedOutput.selections[i].tableName);
    CHECK(data.selections[i].columnName ==
        expectedOutput.selections[i].columnName);
    for (gsl::index j = 0; j < data.selections[i].reqs.size(); ++j) {
      CHECK(data.selections[i].reqs[j] ==
          expectedOutput.selections[i].reqs[j]);
    }
  }

  for (gsl::index i = 0; i < data.joins.size(); ++i) {
    CHECK(data.joins[i].table1Name == expectedOutput.joins[i].table1Name);
    CHECK(data.joins[i].column1Name == expectedOutput.joins[i].column1Name);
    CHECK(data.joins[i].table2Name == expectedOutput.joins[i].table2Name);
    CHECK(data.joins[i].column2Name == expectedOutput.joins[i].column2Name);
  }

  for (gsl::index i = 0; i < data.projections.size(); ++i) {
    CHECK(data.projections[i].columnName
              == expectedOutput.projections[i].columnName);
    CHECK(data.projections[i].tableName
              == expectedOutput.projections[i].tableName);
    CHECK(data.projections[i].operation
              == expectedOutput.projections[i].operation);
  }
}

TEST_CASE("Parse agregation", "[SQLParser]") {
  DataWorkers::QueryData data = DataWorkers::SQLParser::parse(
      "SELECT table1.t1, SUM(table1.t2), AVG(table2.t3) FROM table1");

  CHECK(data.projections[0].operation == Operation::Distinct);
  CHECK(data.projections[1].operation == Operation::Sum);
  CHECK(data.projections[2].operation == Operation::Average);
}