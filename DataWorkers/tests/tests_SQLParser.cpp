//
// Created by Petr Flajsingr on 15/11/2018.
//

#include "gtest/gtest.h"
#include "SQLParser.h"


class SQLParser_tests : public ::testing::Test {
 protected:


 public:
  SQLParser_tests() = default;

  void SetUp() override {
  }

  void TearDown() override {
  }

  ~SQLParser_tests() override = default;
};

TEST_F(SQLParser_tests, parse) {
  DataWorkers::QueryData data = DataWorkers::SQLParser::parse(
      "SELECT table1.uganda, table1.letadlo, table2.krava, table3.id FROM table1 JOIN table2 ON table1.uganda = table2.id JOIN table2 ON table2.id = table3.id WHERE table1.uganda = 10 | 5 | 8 AND table1.letadlo = 22 | 23");
}