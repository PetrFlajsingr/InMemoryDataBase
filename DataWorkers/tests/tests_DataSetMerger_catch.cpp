//
// Created by Petr Flajsingr on 2019-02-08.
//

#include <catch.hpp>
#include <ArrayDataProvider.h>
#include <MemoryDataSet.h>
#include <DataSetMerger.h>

SCENARIO("Merging two data sets", "[DataSetMerger]") {
  GIVEN("Two simple data sets") {
    const std::vector<std::vector<std::string>> test1A{
        {"1", "A", "B", "C", "D"},
        {"2", "10", "1", "COLUMN24", "COLUMN25"},
        {"3", "10", "1", "COLUMN34", "COLUMN35"},
        {"4", "10", "1", "COLUMN44", "COLUMN45"},
        {"5", "20", "1", "COLUMN54", "COLUMN55"},
        {"6", "20", "1", "COLUMN54", "COLUMN55"},
        {"7", "20", "1", "COLUMN54", "COLUMN55"},
        {"8", "20", "1", "COLUMN54", "COLUMN55"}
    };

    const std::vector<std::vector<std::string>> test1B{
        {"8", "10", "1", "COLUMN14", "COLUMN15"},
        {"6", "10", "1", "COLUMN24", "COLUMN25"},
        {"4", "10", "1", "COLUMN34", "COLUMN35"},
        {"2", "10", "1", "COLUMN44", "COLUMN45"},
        {"1", "20", "1", "COLUMN54", "COLUMN55"},
        {"3", "20", "1", "COLUMN54", "COLUMN55"},
        {"5", "20", "1", "COLUMN54", "COLUMN55"},
        {"7", "20", "1", "COLUMN54", "COLUMN55"}
    };

    const std::vector<std::vector<std::string>> test1Result{
        {"1", "A", "B", "C", "D", "20", "1", "COLUMN54", "COLUMN55"},
        {"2", "10", "1", "COLUMN24", "COLUMN25", "10", "1", "COLUMN44",
         "COLUMN45"},
        {"3", "10", "1", "COLUMN34", "COLUMN35", "20", "1", "COLUMN54",
         "COLUMN55"},
        {"4", "10", "1", "COLUMN44", "COLUMN45", "10", "1", "COLUMN34",
         "COLUMN35"},
        {"5", "20", "1", "COLUMN54", "COLUMN55", "20", "1", "COLUMN54",
         "COLUMN55"},
        {"6", "20", "1", "COLUMN54", "COLUMN55", "10", "1", "COLUMN24",
         "COLUMN25"},
        {"7", "20", "1", "COLUMN54", "COLUMN55", "20", "1", "COLUMN54",
         "COLUMN55"},
        {"8", "20", "1", "COLUMN54", "COLUMN55", "10", "1", "COLUMN14",
         "COLUMN15"}
    };

    DataProviders::ArrayDataProvider dataProvider1A(test1A);
    DataSets::MemoryDataSet dataSet1A("A");
    dataSet1A.open(dataProvider1A,
                   {ValueType::String, ValueType::String, ValueType::String,
                    ValueType::String, ValueType::String});

    DataProviders::ArrayDataProvider dataProvider1B(test1B);
    DataSets::MemoryDataSet dataSet1B("B");
    dataSet1B.open(dataProvider1B,
                   {ValueType::String, ValueType::String, ValueType::String,
                    ValueType::String, ValueType::String});

    THEN("The merged data correspond to test results") {
      DataWorkers::DataSetMerger merger;
      merger.addDataSet(&dataSet1A);
      merger.addDataSet(&dataSet1B);
      auto mergeResult = merger.mergeDataSets("A", "B", "0", "0");
      auto fields = mergeResult->getFields();

      for (auto i = 0; i < test1Result.size(); ++i) {
        for (auto j = 0; j < test1Result[i].size(); ++j) {
          REQUIRE(fields[j]->getAsString() == test1Result[i][j]);
        }
        mergeResult->next();
      }
    }
  }
}