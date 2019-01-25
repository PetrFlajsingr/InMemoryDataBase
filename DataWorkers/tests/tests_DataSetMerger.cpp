//
// Created by Petr Flajsingr on 25/01/2019.
//

#include <DataSetMerger.h>
#include <ArrayDataProvider.h>
#include "gtest/gtest.h"

using namespace DataProviders;
using namespace DataSets;
using namespace DataWorkers;

class DataSetMerger_tests : public ::testing::Test {
 protected:

  BaseDataProvider *dataProvider1A = nullptr;
  BaseDataSet *dataSet1A = nullptr;

  BaseDataProvider *dataProvider1B = nullptr;
  BaseDataSet *dataSet1B = nullptr;

  std::vector<std::vector<std::string>> test1A{
      {"1", "A", "B", "C", "D"},
      {"2", "10", "1", "COLUMN24", "COLUMN25"},
      {"3", "10", "1", "COLUMN34", "COLUMN35"},
      {"4", "10", "1", "COLUMN44", "COLUMN45"},
      {"5", "20", "1", "COLUMN54", "COLUMN55"},
      {"6", "20", "1", "COLUMN54", "COLUMN55"},
      {"7", "20", "1", "COLUMN54", "COLUMN55"},
      {"8", "20", "1", "COLUMN54", "COLUMN55"}
  };

  std::vector<std::vector<std::string>> test1B{
      {"8", "10", "1", "COLUMN14", "COLUMN15"},
      {"6", "10", "1", "COLUMN24", "COLUMN25"},
      {"4", "10", "1", "COLUMN34", "COLUMN35"},
      {"2", "10", "1", "COLUMN44", "COLUMN45"},
      {"1", "20", "1", "COLUMN54", "COLUMN55"},
      {"3", "20", "1", "COLUMN54", "COLUMN55"},
      {"5", "20", "1", "COLUMN54", "COLUMN55"},
      {"7", "20", "1", "COLUMN54", "COLUMN55"}
  };

  std::vector<std::vector<std::string>> test1Result{
      {"1", "A", "B", "C", "D", "20", "1", "COLUMN54", "COLUMN55"},
      {"2", "10", "1", "COLUMN24", "COLUMN25", "10", "1", "COLUMN44", "COLUMN45"},
      {"3", "10", "1", "COLUMN34", "COLUMN35", "20", "1", "COLUMN54", "COLUMN55"},
      {"4", "10", "1", "COLUMN44", "COLUMN45", "10", "1", "COLUMN34", "COLUMN35"},
      {"5", "20", "1", "COLUMN54", "COLUMN55", "20", "1", "COLUMN54", "COLUMN55"},
      {"6", "20", "1", "COLUMN54", "COLUMN55", "10", "1", "COLUMN24", "COLUMN25"},
      {"7", "20", "1", "COLUMN54", "COLUMN55", "20", "1", "COLUMN54", "COLUMN55"},
      {"8", "20", "1", "COLUMN54", "COLUMN55", "10", "1", "COLUMN14", "COLUMN15"}
  };

 public:
  DataSetMerger_tests() = default;

  void SetUp() override {
    dataProvider1A = new ArrayDataProvider(test1A);
    dataSet1A = new MemoryDataSet("A");
    dataSet1A->setDataProvider(dataProvider1A);
    dataSet1A->setFieldTypes({STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL});
    dataSet1A->open();

    dataProvider1B = new ArrayDataProvider(test1B);
    dataSet1B = new MemoryDataSet("B");
    dataSet1B->setDataProvider(dataProvider1B);
    dataSet1B->setFieldTypes({STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL});
    dataSet1B->open();
  }

  void TearDown() override {
    delete dataSet1A;
    delete dataSet1B;
  }

  ~DataSetMerger_tests() override = default;
};

TEST_F(DataSetMerger_tests, merge_test) {
  DataSetMerger merger;

  merger.addDataSet(dataSet1A);
  merger.addDataSet(dataSet1B);

  auto mergeResult = merger.mergeDataSets("A", "B", "0", "0");

  auto fields = mergeResult->getFields();

  for (auto i = 0; i < test1Result.size(); ++i) {
    for (auto j = 0; j < test1Result[i].size(); ++j) {
      //std::cout << fields[j]->getAsString() << ", ";
      EXPECT_EQ(fields[j]->getAsString(), test1Result[i][j]);
    }
    //std::cout << std::endl;
    mergeResult->next();
  }
}