//
// Created by Petr Flajsingr on 2019-02-02.
//

#include <catch.hpp>
#include <ArrayDataProvider.h>
#include <MemoryDataSet.h>

#define DATA_COUNT 10000
#define COL_COUNT 5

SCENARIO("Using MemoryDataSet with DataProvider", "[MemoryDataSet]") {
  GIVEN("Provider with 10 000 rows of data") {
    std::vector<std::vector<std::string>> data;
    data.reserve(DATA_COUNT);
    std::vector<std::string> row;
    row.reserve(COL_COUNT);
    for (auto i = 0; i < DATA_COUNT; ++i) {
      auto firstPart = std::to_string(i);
      for (auto j = 0; j < COL_COUNT; ++j) {
        row.emplace_back(firstPart + "_" + std::to_string(j));
      }
      data.emplace_back(row);
      row.clear();
    }

    DataProviders::ArrayDataProvider provider(data);

    WHEN("Reading the data from provider using BaseDataSet interface") {
      DataSets::MemoryDataSet dataSet("test_dataset");
      auto fieldTypes = {
          ValueType::String,
          ValueType::String,
          ValueType::String,
          ValueType::String,
          ValueType::String,
      };
      dataSet.open(provider, fieldTypes);

      THEN("The data inside data set correspond to data read") {
        auto fields = dataSet.getFields();
        REQUIRE(fields.size() == COL_COUNT);

        int rowCount = 0;
        while (dataSet.next()) {
          for (auto field : fields) {
            CHECK(field->getAsString() == data[rowCount][field->getIndex()]);
          }
          rowCount++;
        }
        REQUIRE(rowCount == DATA_COUNT);

        rowCount--;
        while (dataSet.previous()) {
          auto wat = data[rowCount][0];
          for (auto field : fields) {
            CHECK(field->getAsString() == data[rowCount][field->getIndex()]);
          }
          auto wat2 = data[rowCount][0];
          rowCount--;
        }
        REQUIRE(rowCount == -1);
      }

      AND_THEN("The data can be sorted") {
        DataSets::SortOptions sortOptions;
        sortOptions.addOption(0, SortOrder::Descending);
        dataSet.sort(sortOptions);
        auto field0 = dataSet.fieldByIndex(0);
        // "zzz" since there are only string numbers in test data
        std::string lastVal = "zzzz";
        while (dataSet.next()) {
          REQUIRE(lastVal >= field0->getAsString());
          lastVal = field0->getAsString();
        }

        DataSets::SortOptions sortOptions2;
        sortOptions2.addOption(3, SortOrder::Ascending);
        dataSet.sort(sortOptions2);
        auto field3 = dataSet.fieldByIndex(3);

        lastVal = "";
        while (dataSet.next()) {
          REQUIRE(lastVal <= field3->getAsString());
          lastVal = field3->getAsString();
        }
      }

      AND_THEN("The data can be filtered") {
        FAIL("TODO: implement");
      }
    }

    WHEN("Reading the data from provider using iterator") {
      THEN("The data inside data set correspond to data read") {
        FAIL("TODO: implement");
      }
    }
  }
}