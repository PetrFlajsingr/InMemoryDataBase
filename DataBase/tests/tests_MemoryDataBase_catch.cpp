//
// Created by Igor Frank on 04.10.19.
//

#include "testTableString.h"
#include <ArrayDataProvider.h>
#include <CsvReader.h>
#include <CurrencyField.h>
#include <DoubleField.h>
#include <IntegerField.h>
#include <MemoryDataBase.h>
#include <MemoryDataSet.h>
#include <catch2/catch.hpp>
#include <stdexcept>

#define TABLE1_COLUMN_COUNT 3
#define TABLE1_ROW_COUNT 3

SCENARIO("Operations with DB", "[MemoryDataBase]") {
  GIVEN("One table") {

    DataProviders::ArrayDataProvider dataProvider(TestTables::testTable1);
    DataBase::MemoryDataBase db("testDB");

    auto dsTable1 = std::make_shared<DataSets::MemoryDataSet>("table1");
    dsTable1->open(dataProvider,
                   {ValueType::Integer, ValueType::Double, ValueType::Currency, ValueType::String, ValueType::String,
                    ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String});
    db.addTable(dsTable1);

    WHEN("Selecting whole table") {
      const std::string query = "SELECT table1.* FROM table1;";

      THEN("Correct values are in table") {
        auto result = db.execSimpleQuery(query, false, "table1View");
        auto rowNumber = 1;
        while (result->dataSet->next()) {
          CHECK(dynamic_cast<DataSets::IntegerField *>(result->dataSet->fieldByIndex(0))->getAsInteger() ==
                Utilities::stringToInt(TestTables::testTable1[rowNumber][0]));
          CHECK(Utilities::compareDouble(
                    dynamic_cast<DataSets::DoubleField *>(result->dataSet->fieldByIndex(1))->getAsDouble(),
                    Utilities::stringToDouble(TestTables::testTable1[rowNumber][1])) == 0);
          CHECK(Utilities::compareCurrency(
                    dynamic_cast<DataSets::CurrencyField *>(result->dataSet->fieldByIndex(2))->getAsCurrency(),
                    Currency(TestTables::testTable1[rowNumber][2])) == 0);
          for (auto i = 3; i < static_cast<int>(TestTables::testTable1[0].size()); ++i) {
            CHECK(result->dataSet->fieldByIndex(i)->getAsString() == TestTables::testTable1[rowNumber][i]);
          }
          ++rowNumber;
        }
      }
    }
    try {
      WHEN("Selecting one column") {
        const std::string query = "SELECT table1.B FROM table1;";
        THEN("Correct column returned") {
          auto result = db.execSimpleQuery(query, true, "table1View2");
          auto rowNumber = 1;
          while (result->dataSet->next()) {
            auto fields = result->dataSet->getFields();
            REQUIRE(result->dataSet->getColumnCount() == 1);
            for (auto field : fields) {
              CHECK(Utilities::compareDouble(dynamic_cast<DataSets::DoubleField *>(field)->getAsDouble(),
                                             Utilities::stringToDouble(TestTables::testTable1[rowNumber][1])) == 0);
            }
            ++rowNumber;
          }
        }
      }
    } catch (const std::exception &e) {
      std::cout << e.what();
    }
  }
}
