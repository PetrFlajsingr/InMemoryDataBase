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

  std::vector<std::vector<std::string>> test{{"COLUMN11", "COLUMN12", "COLUMN13", "COLUMN14", "COLUMN15"},
                                             {"COLUMN21", "COLUMN22", "COLUMN23", "COLUMN24", "COLUMN25"},
                                             {"COLUMN31", "COLUMN32", "COLUMN33", "COLUMN34", "COLUMN35"},
                                             {"COLUMN41", "COLUMN42", "COLUMN43", "COLUMN44", "COLUMN45"},
                                             {"COLUMN51", "COLUMN52", "COLUMN53", "COLUMN54", "COLUMN55"}
  };

 public:
  DataWorker_tests() = default;

  void SetUp() override {
    dataProvider = new ArrayDataProvider(test);
  }

  void TearDown() override {
  }

  ~DataWorker_tests() override = default;
};

TEST_F(DataWorker_tests, basicCreate) {
  EXPECT_NO_THROW(worker = new FINMDataWorker(dataProvider,
                                              {STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL}));

  delete worker;
}

TEST_F(DataWorker_tests, choices) {
  EXPECT_NO_THROW(worker = new FINMDataWorker(dataProvider,
                                              {STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL}));

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
                                              {STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL}));

  std::vector<ColumnOperation> ops;
  ColumnOperation op;
  op.operation = Distinct;
  op.columnName = "0";
  ops.push_back(op);

  worker->setColumnOperations(ops);

  ArrayWriter writer;
  writer.result = new std::vector<std::string>();

  worker->writeResult(writer);

  EXPECT_EQ((*writer.result)[0], "0");
  for (int i = 1; i < writer.result->size(); ++i) {
    EXPECT_EQ((*writer.result)[i], test[i - 1][0]);
  }

  delete worker;
}

TEST_F(DataWorker_tests, basicSum) {
  EXPECT_NO_THROW(worker = new FINMDataWorker(dataProvider,
                                              {STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL}));


  delete worker;
}

TEST_F(DataWorker_tests, basicAverage) {
  EXPECT_NO_THROW(worker = new FINMDataWorker(dataProvider,
                                              {STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL}));


  delete worker;
}
