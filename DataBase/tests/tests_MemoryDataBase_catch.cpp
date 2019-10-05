//
// Created by Igor Frank on 04.10.19.
//

#include <CsvReader.h>
#include <ArrayDataProvider.h>
#include <catch2/catch.hpp>
#include <MemoryDataBase.h>
#include <MemoryDataSet.h>
#include <stdexcept>
#include "testTableString.h"
#include <DoubleField.h>
#include <CurrencyField.h>
#include <IntegerField.h>

#define TABLE1_COLUMN_COUNT 3
#define TABLE1_ROW_COUNT 3

SCENARIO("Operations with DB", "[MemoryDataBase]") {
    GIVEN("Basic tables") {

        const std::string query = "SELECT table1.* FROM table1;";

        DataProviders::ArrayDataProvider dataProvider(TestTables::testTable1);
        DataBase::MemoryDataBase db("testDB");

        WHEN("Adding table") {
            auto dsTable1 = std::make_shared<DataSets::MemoryDataSet>("table1");
            dsTable1->open(dataProvider, {ValueType::Integer,
                                          ValueType::Double,
                                          ValueType::Currency,
                                          ValueType::String,
                                          ValueType::String,
                                          ValueType::String,
                                          ValueType::String,
                                          ValueType::String,
                                          ValueType::String,
                                          ValueType::String});
            db.addTable(dsTable1);

            AND_THEN("Correct field types values are in table") {
                auto result = db.execSimpleQuery(query, false, "table1View");
                auto rowCount = 0;
                while (result->dataSet->next()) {
                    REQUIRE(result->dataSet->fieldByIndex(0)->getFieldType() == ValueType::Integer);
                    CHECK(dynamic_cast<DataSets::IntegerField *>(result->dataSet->fieldByIndex(0))->getAsInteger() ==
                          Utilities::stringToInt(TestTables::testTable1[rowCount][0]));

                    REQUIRE(result->dataSet->fieldByIndex(1)->getFieldType() == ValueType::Double);
                    CHECK(Utilities::compareDouble(
                            dynamic_cast<DataSets::DoubleField *>(result->dataSet->fieldByIndex(1))->getAsDouble(),
                            Utilities::stringToDouble(TestTables::testTable1[rowCount][1])) == 0);

                    REQUIRE(result->dataSet->fieldByIndex(2)->getFieldType() == ValueType::Currency);
                    CHECK(Utilities::compareCurrency(
                            dynamic_cast<DataSets::CurrencyField *>(result->dataSet->fieldByIndex(2))->getAsCurrency(),
                            Currency(TestTables::testTable1[rowCount][2])) == 0);

                    for (auto i = 3; i < static_cast<int>(TestTables::testTable1[0].size()); ++i) {
                        REQUIRE(result->dataSet->fieldByIndex(i)->getFieldType() == ValueType::String);
                        CHECK(result->dataSet->fieldByIndex(i)->getAsString() == TestTables::testTable1[rowCount][i]);
                    }
                    ++rowCount;
                }
            }
        }
    }
}

