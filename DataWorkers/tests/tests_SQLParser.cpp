//
// Created by Petr Flajsingr on 15/11/2018.
//

#include "gtest/gtest.h"
#include "SQLParser.h"


class SQLParser_tests : public ::testing::Test {
 protected:
  DataWorkers::QueryData expectedOutput;

 public:
  SQLParser_tests() = default;

  void SetUp() override {
    expectedOutput.projections.push_back({"uganda", "table1", DataWorkers::Operation::Distinct});
    expectedOutput.projections.push_back({"letadlo", "table1", DataWorkers::Operation::Distinct});
    expectedOutput.projections.push_back({"krava", "table2", DataWorkers::Operation::Distinct});
    expectedOutput.projections.push_back({"id", "table3", DataWorkers::Operation::Distinct});

    expectedOutput.joins.push_back({"table1", "uganda", "table2", "id"});
    expectedOutput.joins.push_back({"table2", "id", "table3", "id"});

    expectedOutput.selections.push_back({"table1", "uganda", {"10", "5", "8"}});
    expectedOutput.selections.push_back({"table1", "letadlo", {"22", "23"}});
  }

  void TearDown() override {
  }

  ~SQLParser_tests() override = default;
};

TEST_F(SQLParser_tests, parse) {
  DataWorkers::QueryData data = DataWorkers::SQLParser::parse(
      "SELECT table1.uganda, table1.letadlo, table2.krava, table3.id FROM table1 JOIN table2 ON table1.uganda = table2.id JOIN table3 ON table2.id = table3.id WHERE table1.uganda = 10 | 5 | 8 AND table1.letadlo = 22 | 23");

  ASSERT_EQ(data.selections.size(), expectedOutput.selections.size());
  ASSERT_EQ(data.projections.size(), expectedOutput.projections.size());
  ASSERT_EQ(data.joins.size(), expectedOutput.joins.size());

  for (int i = 0; i < data.selections.size(); ++i) {
    EXPECT_EQ(data.selections[i].tableName,
        expectedOutput.selections[i].tableName);
    EXPECT_EQ(data.selections[i].columnName,
        expectedOutput.selections[i].columnName);
    for(int j = 0; j < data.selections[i].reqs.size(); ++j) {
      EXPECT_EQ(data.selections[i].reqs[j],
          expectedOutput.selections[i].reqs[j]);
    }
  }

  for (int i = 0; i < data.joins.size(); ++i) {
    EXPECT_EQ(data.joins[i].table1Name, expectedOutput.joins[i].table1Name);
    EXPECT_EQ(data.joins[i].column1Name, expectedOutput.joins[i].column1Name);
    EXPECT_EQ(data.joins[i].table2Name, expectedOutput.joins[i].table2Name);
    EXPECT_EQ(data.joins[i].column2Name, expectedOutput.joins[i].column2Name);
  }

  for (int i = 0; i < data.projections.size(); ++i) {
    EXPECT_EQ(data.projections[i].columnName, expectedOutput.projections[i].columnName);
    EXPECT_EQ(data.projections[i].tableName, expectedOutput.projections[i].tableName);
    EXPECT_EQ(data.projections[i].operation, expectedOutput.projections[i].operation);
  }
}

TEST_F(SQLParser_tests, parseAggregation) {
  DataWorkers::QueryData data = DataWorkers::SQLParser::parse(
      "SELECT table1.t1, SUM(table1.t2), AVG(table2.t3) FROM table1");

  EXPECT_EQ(data.projections[0].operation, DataWorkers::Distinct);
  EXPECT_EQ(data.projections[1].operation, DataWorkers::Sum);
  EXPECT_EQ(data.projections[2].operation, DataWorkers::Average);

}