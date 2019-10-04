//
// Created by Igor Frank on 04.10.19.
//

#include <CsvReader.h>
#include <catch2/catch.hpp>
#include <MemoryDataBase.h>
#include <MemoryDataSet.h>

#define TABLE1_COLUMN_COUNT 3
#define TABLE1_ROW_COUNT 3

SCENARIO("Operations with DB", "[MemoryDataBase]"){
    GIVEN("Basic tables"){
        const std::string dbData[TABLE1_ROW_COUNT][TABLE1_COLUMN_COUNT]{
                {"0","\"Johny\"","\"Boy\""},
                {"1","\"Peter\"","\"Griffin\""},
                {"2","\"Bruce\"","\"Don\""}
        };

        const std::string query = "SELECT table1.* FROM table1;";

        DataProviders::CsvReader csvReader("../DataBase/tests/files/basic_table.csv",
                                           ",");
        DataBase::MemoryDataBase db("testDB");

        WHEN("Adding table"){
            auto dsTable1 = std::make_shared<DataSets::MemoryDataSet>("table1");
            dsTable1->open(csvReader, {ValueType::Integer,
                                       ValueType::String,
                                       ValueType::String});
            db.addTable(dsTable1);

            THEN("Correct values are in table"){
                auto result = db.execSimpleQuery(query, false, "table1View");
                auto rowCount = 0;
                while(result->dataSet->next()){
                    for(auto i = 0; i < TABLE1_COLUMN_COUNT; ++i) {
                       REQUIRE(result->dataSet->fieldByIndex(i)->getAsString() == dbData[rowCount][i]);
                    }
                    ++rowCount;
                }
            }
        }
    }
}

