//
// Created by Petr Flajsingr on 30/08/2018.
//

#include <BaseDataWorker.h>
#include <MemoryDataWorker.h>
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
    {"COLUMN51", "20", "1", "COLUMN54", "COLUMN55"},
    {"COLUMN22", "20", "1", "COLUMN54", "COLUMN55"},
    {"COLUMN22", "20", "1", "COLUMN54", "COLUMN55"},
    {"COLUMN22", "20", "1", "COLUMN54", "COLUMN55"}
  };

  std::vector<std::vector<std::string>> basicTest {
      {"COLUMN11", "10", "1", "COLUMN14", "COLUMN15"},
      {"COLUMN21", "10", "1", "COLUMN24", "COLUMN25"},
      {"COLUMN31", "10", "1", "COLUMN34", "COLUMN35"},
      {"COLUMN41", "10", "1", "COLUMN44", "COLUMN45"},
      {"COLUMN51", "20", "1", "COLUMN54", "COLUMN55"},
  };

  std::vector<std::vector<std::string>> joinTest {
      {"10", "deset"},
      {"20", "dvacet"},
  };

  std::vector<std::vector<std::string>> mixedTest {
      {"0", "1", "2(Sum)", "3", "4", "1"},
      {"COLUMN11", "10", "1", "COLUMN14", "COLUMN15"},
      {"COLUMN21", "10", "1", "COLUMN24", "COLUMN25"},
      {"COLUMN22", "20", "3", "COLUMN54", "COLUMN55"},
      {"COLUMN31", "10", "1", "COLUMN34", "COLUMN35"},
      {"COLUMN41", "10", "1", "COLUMN44", "COLUMN45"},
      {"COLUMN51", "20", "1", "COLUMN54", "COLUMN55"}
  };

  std::vector<std::vector<std::string>> joinAnswers {
      {"0", "1", "2(Sum)", "3", "4", "1"},
      {"COLUMN11", "10", "1", "COLUMN14", "COLUMN15", "deset"},
      {"COLUMN21", "10", "1", "COLUMN24", "COLUMN25", "deset"},
      {"COLUMN22", "20", "3", "COLUMN54", "COLUMN55", "dvacet"},
      {"COLUMN31", "10", "1", "COLUMN34", "COLUMN35", "deset"},
      {"COLUMN41", "10", "1", "COLUMN44", "COLUMN45", "deset"},
      {"COLUMN51", "20", "1", "COLUMN54", "COLUMN55", "dvacet"}
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

  std::vector<std::string> advUnique0 {
      "ctvrty",
      "druhy",
      "prvni",
      "special",
      "treti",
  };

  std::vector<std::string> advUnique1 {
      "11",
      "22",
      "33",
      "44",
      "88",
      "2222",
      "2331",
      "2422",
      "2839",
      "22222",
  };

  std::vector<std::vector<std::string>> advAnswers {
      {"0", "1", "2(Sum)", "3(Sum)", "4(Sum)", "5(Sum)"},
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
  EXPECT_NO_THROW(worker = new MemoryDataWorker(dataProvider,
                                                {STRING_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL, STRING_VAL}));

  delete worker;
}

TEST_F(DataWorker_tests, choices) {
  EXPECT_NO_THROW(worker = new MemoryDataWorker(dataProvider,
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
  auto provider = new DataProviders::ArrayDataProvider(basicTest);

  EXPECT_NO_THROW(worker = new MemoryDataWorker(provider,
                                                {STRING_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL, STRING_VAL}));

  std::string sql = "SELECT main.0 FROM main";

  ArrayWriter writer;
  writer.result = new std::vector<std::vector<std::string>>();

  worker->writeResult(writer, sql);

  EXPECT_EQ((*writer.result)[0][0], "0");
  for (int i = 1; i < writer.result->size(); ++i) {
    EXPECT_EQ((*writer.result)[i][0], basicTest[i - 1][0]);
  }

  delete worker;
  delete provider;
}

TEST_F(DataWorker_tests, advancedDinstinct) {
  EXPECT_NO_THROW(worker = new MemoryDataWorker(advDataProvider,
                                                {STRING_VAL, INTEGER_VAL, CURRENCY, CURRENCY, INTEGER_VAL, CURRENCY}));

  std::string sql = "SELECT main.0, main.1, SUM(main.2), SUM(main.3), SUM(main.4), SUM(main.5) FROM main";

  ArrayWriter writer;
  writer.result = new std::vector<std::vector<std::string>>();

  worker->writeResult(writer, sql);

  for (int i = 0; i < advTest[0].size(); ++i) {
    EXPECT_EQ((*writer.result)[0][i], advAnswers[0][i]);
  }

  delete worker;
}

TEST_F(DataWorker_tests, basicSum) {
  auto provider = new DataProviders::ArrayDataProvider(basicTest);

  EXPECT_NO_THROW(worker = new MemoryDataWorker(provider,
                                                {STRING_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL, STRING_VAL}));

  std::string sql = "SELECT main.0, SUM(main.1) FROM main";

  ArrayWriter writer;
  writer.result = new std::vector<std::vector<std::string>>();

  worker->writeResult(writer, sql);

  EXPECT_EQ((*writer.result)[0][1], "1(Sum)");
  for (int i = 1; i < writer.result->size(); ++i) {
    EXPECT_EQ((*writer.result)[i][1], basicTest[i - 1][1]);
  }

  delete worker;
  delete provider;
}

TEST_F(DataWorker_tests, advancedSum) {
  EXPECT_NO_THROW(worker = new MemoryDataWorker(advDataProvider,
                                                {STRING_VAL, INTEGER_VAL, CURRENCY, CURRENCY, INTEGER_VAL, CURRENCY}));


  std::string sql = "SELECT main.0, main.1, SUM(main.2), SUM(main.3), SUM(main.4), SUM(main.5) FROM main";

  ArrayWriter writer;
  writer.result = new std::vector<std::vector<std::string>>();

  worker->writeResult(writer, sql);

  for (int i = 0; i < (*writer.result).size(); ++i) {
    for (int j = 0; j < (*writer.result)[i].size();++j) {
      EXPECT_STREQ((*writer.result)[i][j].c_str(), advAnswers[i][j].c_str());
    }
  }

  delete worker;
}

TEST_F(DataWorker_tests, basicAverage) {
  auto provider = new DataProviders::ArrayDataProvider(basicTest);

  EXPECT_NO_THROW(worker = new MemoryDataWorker(provider,
                                                {STRING_VAL, INTEGER_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL}));

  std::string sql = "SELECT main.0, AVG(main.2) FROM main";

  ArrayWriter writer;
  writer.result = new std::vector<std::vector<std::string>>();

  worker->writeResult(writer, sql);

  EXPECT_EQ((*writer.result)[0][1], "2(Avg)");
  for (int i = 1; i < writer.result->size(); ++i) {
    EXPECT_EQ((*writer.result)[i][1], basicTest[i - 1][2]);
  }

  delete worker;
  delete provider;
}

TEST_F(DataWorker_tests, selection) {
  EXPECT_NO_THROW(worker = new MemoryDataWorker(advDataProvider,
                                                {STRING_VAL, INTEGER_VAL, CURRENCY, CURRENCY, INTEGER_VAL, CURRENCY}));

  std::string sql = "SELECT main.0, AVG(main.2) FROM main WHERE main.0 = prvni | druhy";

  ArrayWriter writer;
  writer.result = new std::vector<std::vector<std::string>>();

  worker->writeResult(writer, sql);

  EXPECT_EQ((*writer.result)[0][0], "0");
  for (int i = 1; i < writer.result->size(); ++i) {
    EXPECT_TRUE((*writer.result)[i][0] == "prvni" or
      (*writer.result)[i][0] == "druhy");
  }

  delete worker;
}

TEST_F(DataWorker_tests, mix) {
  EXPECT_NO_THROW(worker = new MemoryDataWorker(dataProvider,
                                                {STRING_VAL, INTEGER_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL}));

    std::string sql = "SELECT main.0, main.1, SUM(main.2), main.3, main.4 FROM main";

    auto joinProvider = new DataProviders::ArrayDataProvider(joinTest);

    ArrayWriter writer;
    writer.result = new std::vector<std::vector<std::string>>();

    worker->writeResult(writer, sql);

    for (int i = 0; i < (*writer.result).size(); ++i) {
      for (int j = 0; j < (*writer.result)[i].size(); ++j) {
        EXPECT_STREQ((*writer.result)[i][j].c_str(), mixedTest[i][j].c_str());
      }
    }

    delete worker;
}

TEST_F(DataWorker_tests, join) {
  EXPECT_NO_THROW(worker = new MemoryDataWorker(dataProvider,
                                                {STRING_VAL, INTEGER_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL}));

  std::string sql = "SELECT main.0, main.1, SUM(main.2), main.3, main.4, joinTest.1 FROM main JOIN joinTest ON main.1 = joinTest.0";

  auto joinProvider = new DataProviders::ArrayDataProvider(joinTest);
  auto joinDataSet = new DataSets::MemoryDataSet("joinTest");
  joinDataSet->setDataProvider(joinProvider);
  joinDataSet->setFieldTypes({INTEGER_VAL, STRING_VAL});
  joinDataSet->open();

  worker->addDataSet(joinDataSet);

  ArrayWriter writer;
  writer.result = new std::vector<std::vector<std::string>>();

  worker->writeResult(writer, sql);

  writer.print();

  for (int i = 0; i < (*writer.result).size(); ++i) {
    for (int j = 0; j < (*writer.result)[i].size(); ++j) {
      EXPECT_STREQ((*writer.result)[i][j].c_str(), joinAnswers[i][j].c_str());
    }
  }

  delete worker;
}

TEST_F(DataWorker_tests, uniqueValues) {
  EXPECT_NO_THROW(worker = new MemoryDataWorker(advDataProvider,
                                                {STRING_VAL, INTEGER_VAL, CURRENCY, CURRENCY, INTEGER_VAL, CURRENCY}));

  std::vector<std::string> cols = {"0", "1"};
  worker->setColumnChoices(cols);

  auto unique0 = worker->getChoices("0");
  auto unique1 = worker->getChoices("1");

  EXPECT_EQ(unique0.size(), advUnique0.size());
  for (int i = 0; i < advUnique0.size(); i++) {
    EXPECT_STREQ(advUnique0[i].c_str(), unique0[i].c_str());
  }

  EXPECT_EQ(unique1.size(), advUnique1.size());
  for (int i = 0; i < advUnique1.size(); i++) {
    EXPECT_STREQ(advUnique1[i].c_str(), unique1[i].c_str());
  }

  delete worker;
}
