//
// Created by Petr Flajsingr on 30/08/2018.
//

#include <BaseDataWorker.h>
#include <FINMDataWorker.h>
#include <ArrayWriter.h>
#include "gtest/gtest.h"
#include "MemoryDataSet.h"
#include "ArrayDataProvider.h"

using namespace DataWorkers;
using namespace DataProviders;

class DataWorker_tests : public ::testing::Test {
 protected:
  BaseDataWorker *worker = nullptr;

  BaseDataProvider *dataProvider = nullptr;

  BaseDataProvider *advDataProvider = nullptr;

  std::vector<std::vector<std::string>> test{
    {"COLUMN11", "10", "1", "COLUMN14", "COLUMN15"},
    {"COLUMN21", "10", "1", "COLUMN24", "COLUMN25"},
    {"COLUMN31", "10", "1", "COLUMN34", "COLUMN35"},
    {"COLUMN41", "10", "1", "COLUMN44", "COLUMN45"},
    {"COLUMN51", "10", "1", "COLUMN54", "COLUMN55"}
  };

  std::vector<std::vector<std::string>> advTest{
      {"prvni", "2839", "10.01", "444.8", "27", "8"},
      {"special", "33", "90.00", "666.8", "27", "2"},
      {"special", "44", "90.00", "666.8", "27", "2"},
      {"druhy", "2422", "200.02", "222.8", "27", "3"},
      {"prvni", "2839", "3000.03", "333.8", "27", "9"},
      {"treti", "2331", "40000.04", "444.8", "27", "11"},
      {"ctvrty", "22222", "90.00", "666.8", "27", "2"},
      {"druhy", "2839", "500000.05", "555.8", "27", "5"},
      {"prvni", "2222", "600000.06", "111.8", "27", "6"},
      {"ctvrty", "2839", "70000.07", "222.8", "27", "1"},
      {"prvni", "2222", "8000.08", "333.8", "27", "7"},
      {"treti", "88", "900.09", "555.8", "27", "10"},
      {"special", "11", "1.00", "666.8", "27", "2"},
      {"special", "22", "1.00", "666.8", "27", "2"},
      {"special", "22", "1.00", "666.8", "27", "2"},
      {"special", "11", "1.00", "666.8", "27", "2"},
      {"special", "11", "1.00", "666.8", "27", "2"},
  };

  std::vector<std::vector<std::string>> advAnswers {
      {"0", "1(sum)", "2(sum)", "3(sum)", "4(sum)", "5(sum)"},
      {"ctvrty", "2839", "70000.07", "222.80", "27", "1.00"},
      {"ctvrty", "22222", "90.00", "666.80", "27", "2.00"},
      {"druhy", "2422", "200.02", "222.80", "27", "3.00"},
      {"druhy", "2839", "500000.05", "555.80", "27", "5.00"},
      {"prvni", "2222", "608000.14", "445.60", "54", "13.00"},
      {"prvni", "2839", "3010.04", "778.60", "54", "17.00"},
      {"special", "11", "3.00", "2000.40", "81", "6.00"},
      {"special", "22", "2.00", "1333.60", "54", "4.00"},
      {"special", "33", "90.00", "666.80", "27", "2.00"},
      {"special", "44", "90.00", "666.80", "27", "2.00"},
      {"treti", "88", "900.09", "555.80", "27", "10.00"},
      {"treti", "2331", "40000.04", "444.80", "27", "11.00"},
  };

 public:
  DataWorker_tests() = default;

  void SetUp() override {
    dataProvider = new ArrayDataProvider(test);
    advDataProvider = new ArrayDataProvider(advTest);
  }

  void TearDown() override {
  }

  ~DataWorker_tests() override = default;
};

TEST_F(DataWorker_tests, basicCreate) {
  EXPECT_NO_THROW(worker = new FINMDataWorker(dataProvider,
                                              {STRING_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL, STRING_VAL}));

  delete worker;
}

TEST_F(DataWorker_tests, choices) {
  EXPECT_NO_THROW(worker = new FINMDataWorker(dataProvider,
                                              {STRING_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL, STRING_VAL}));

  std::vector<std::string> choices {"COLUMN11", "COLUMN13"};
  worker->setColumnChoices(choices);

  int i = 0;
  for (auto &str : worker->getMultiChoiceNames()) {
    EXPECT_EQ(str, choices[i]);
    ++i;
  }

  delete worker;
}

TEST_F(DataWorker_tests, basicDistinct) {
  EXPECT_NO_THROW(worker = new FINMDataWorker(dataProvider,
                                              {STRING_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL, STRING_VAL}));

  std::vector<SelectionOperation> ops;
  SelectionOperation op;
  op.operation = Distinct;
  op.columnName = "0";
  ops.push_back(op);

  worker->setColumnOperations(ops);

  ArrayWriter writer;
  writer.result = new std::vector<std::vector<std::string>>();

  worker->writeResult(writer);

  EXPECT_EQ((*writer.result)[0][0], "0");
  for (int i = 1; i < writer.result->size(); ++i) {
    EXPECT_EQ((*writer.result)[i][0], test[i - 1][0]);
  }

  delete worker;
}

TEST_F(DataWorker_tests, advancedDinstinct) {
  EXPECT_NO_THROW(worker = new FINMDataWorker(advDataProvider,
                                              {STRING_VAL, INTEGER_VAL, CURRENCY, CURRENCY, INTEGER_VAL, CURRENCY}));

  std::vector<SelectionOperation> ops;
  SelectionOperation op;
  op.operation = Distinct;
  op.columnName = "0";
  ops.push_back(op);
  op.operation = Distinct;
  op.columnName = "1";
  ops.push_back(op);
  op.operation = Sum;
  op.columnName = "2";
  ops.push_back(op);
  op.operation = Sum;
  op.columnName = "3";
  ops.push_back(op);
  op.operation = Sum;
  op.columnName = "4";
  ops.push_back(op);
  op.operation = Sum;
  op.columnName = "5";
  ops.push_back(op);

  worker->setColumnOperations(ops);

  ArrayWriter writer;
  writer.result = new std::vector<std::vector<std::string>>();

  worker->writeResult(writer);

  for (int i = 0; i < advTest[0].size(); ++i) {
    EXPECT_EQ((*writer.result)[0][i], std::to_string(i));
  }

  delete worker;
}

TEST_F(DataWorker_tests, basicSum) {
  EXPECT_NO_THROW(worker = new FINMDataWorker(dataProvider,
                                              {STRING_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL, STRING_VAL}));

  std::vector<SelectionOperation> ops;
  SelectionOperation op1;
  op1.operation = Distinct;
  op1.columnName = "0";
  ops.push_back(op1);
  SelectionOperation op2;
  op2.operation = Sum;
  op2.columnName = "1";
  ops.push_back(op2);

  worker->setColumnOperations(ops);

  ArrayWriter writer;
  writer.result = new std::vector<std::vector<std::string>>();

  worker->writeResult(writer);

  EXPECT_EQ((*writer.result)[0][1], "1");
  for (int i = 1; i < writer.result->size(); ++i) {
    EXPECT_EQ((*writer.result)[i][1], test[i - 1][1]);
  }

  delete worker;
}

TEST_F(DataWorker_tests, advancedSum) {
  EXPECT_NO_THROW(worker = new FINMDataWorker(advDataProvider,
                                              {STRING_VAL, INTEGER_VAL, CURRENCY, CURRENCY, INTEGER_VAL, CURRENCY}));

  std::vector<SelectionOperation> ops;
  SelectionOperation op;
  op.operation = Distinct;
  op.columnName = "0";
  ops.push_back(op);
  op.operation = Distinct;
  op.columnName = "1";
  ops.push_back(op);
  op.operation = Sum;
  op.columnName = "2";
  ops.push_back(op);
  op.operation = Sum;
  op.columnName = "3";
  ops.push_back(op);
  op.operation = Sum;
  op.columnName = "4";
  ops.push_back(op);
  op.operation = Sum;
  op.columnName = "5";
  ops.push_back(op);

  worker->setColumnOperations(ops);

  ArrayWriter writer;
  writer.result = new std::vector<std::vector<std::string>>();

  worker->writeResult(writer);

  for (int i = 0; i < (*writer.result).size(); ++i) {
    for (int j = 0; j < (*writer.result)[i].size();++j) {
      EXPECT_STREQ((*writer.result)[i][j].c_str(), advAnswers[i][j].c_str());
    }
  }

  delete worker;
}

TEST_F(DataWorker_tests, basicAverage) {
  EXPECT_NO_THROW(worker = new FINMDataWorker(dataProvider,
                                              {STRING_VAL, INTEGER_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL}));

  std::vector<SelectionOperation> ops;
  SelectionOperation op1;
  op1.operation = Distinct;
  op1.columnName = "0";
  ops.push_back(op1);
  SelectionOperation op2;
  op2.operation = Average;
  op2.columnName = "2";
  ops.push_back(op2);

  worker->setColumnOperations(ops);

  ArrayWriter writer;
  writer.result = new std::vector<std::vector<std::string>>();

  worker->writeResult(writer);

  EXPECT_EQ((*writer.result)[0][1], "2");
  for (int i = 1; i < writer.result->size(); ++i) {
    EXPECT_EQ((*writer.result)[i][1], test[i - 1][2]);
  }

  delete worker;
}
