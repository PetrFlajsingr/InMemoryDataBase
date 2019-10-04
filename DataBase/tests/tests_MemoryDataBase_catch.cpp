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

            THEN("Correct values are in table") {
                auto result = db.execSimpleQuery(query, false, "table1View");
                auto rowCount = 0;
                while (result->dataSet->next()) {
                    for (auto i = 0; i < static_cast<int>(TestTables::testTable1[0].size()); ++i) {
                        CHECK(result->dataSet->fieldByIndex(i)->getAsString() == TestTables::testTable1[rowCount][i]);
                    }
                    ++rowCount;
                }
            }
        }
    }
}

