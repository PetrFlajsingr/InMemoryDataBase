//
// Created by Igor Frank on 04.10.19.
//

//
// Created by Igor Frank on 03.10.19.
//

#include <ArrayDataProvider.h>
#include <catch2/catch.hpp>
#include <ArrayWriter.h>
#include <csvstream.h>

#define SMALL_COLUMN_COUNT 5
#define SMALL_ROW_COUNT 3
#define ADV_COLUMN_COUNT 3
#define ADV_ROW_COUNT 2

SCENARIO("Reading string array using via ArrayDataProvider interface", "[ArrayDataProvider]"){
    GIVEN("Simple array"){
        const std::vector<std::vector<std::string>> recordsSmall{
                {"COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"},
                {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
                {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
                {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
        };

        DataProviders::ArrayDataProvider dataProvider(recordsSmall);

        WHEN("reading the header") {

            THEN("header is the same as test value") {
                REQUIRE(dataProvider.getHeader().size() == SMALL_COLUMN_COUNT);
                int i = 0;
                for (const auto &value : dataProvider.getHeader()) {
                    CHECK(value == recordsSmall[0][i]);
                    i++;
                }
            }
        }

        WHEN("reading the array contents") {

            THEN("values correspond to test values") {
                int recordCount = 1;
                while (dataProvider.next()) {
                    REQUIRE(dataProvider.getRow().size() == SMALL_COLUMN_COUNT);
                    for (auto j = 0; j < SMALL_COLUMN_COUNT; ++j) {
                        CHECK(dataProvider.getRow()[j] == recordsSmall[recordCount][j]);
                    }
                    ++recordCount;
                }
                REQUIRE(recordCount-1 == SMALL_ROW_COUNT);
            }
        }

        WHEN("restarting the reading") {

            THEN("the read contents are the same") {
                std::vector<std::vector<std::string>> toCheck;
                while (dataProvider.next()) {
                    std::vector<std::string> row;
                    std::copy(dataProvider.getRow().begin(),
                              dataProvider.getRow().end(),
                              std::back_inserter(row));
                    toCheck.emplace_back(row);
                }

                dataProvider.first();

                int recordCount = 1;
                while (dataProvider.next()) {
                    REQUIRE(dataProvider.getRow().size() == SMALL_COLUMN_COUNT);
                    for (int j = 0; j < toCheck[recordCount].size(); ++j) {
                        CHECK(dataProvider.getRow()[j] == toCheck[recordCount][j]);
                    }
                    recordCount++;
                }
                REQUIRE(recordCount-1 == SMALL_ROW_COUNT);
            }
        }

        WHEN("Reading column name and count"){
            THEN("Column names and count are matching"){
                REQUIRE(dataProvider.getColumnCount() == SMALL_COLUMN_COUNT);
                for(auto i =0; i < SMALL_COLUMN_COUNT; ++i){
                    REQUIRE((dataProvider.getColumnName(i)) == recordsSmall[0][i]);
                }
            }
        }
    }

    GIVEN("A bit complex array") {
        const std::vector<std::vector<std::string>> advancedRecord{
                {"A       ", "B", "C"},
                {"\"Ah, oj\"", "B", "C"},
                {"\"Ah, oj\"", "B", R"("test man"", jojo")"},
        };

        DataProviders::ArrayDataProvider dataProvider(advancedRecord);

        WHEN("reading the header") {

            THEN("header is the same as test value") {
                REQUIRE(dataProvider.getHeader().size() == ADV_COLUMN_COUNT);
                int i = 0;
                for (const auto &value : dataProvider.getHeader()) {
                    CHECK(value == advancedRecord[0][i]);
                    i++;
                }
            }
        }

        WHEN("reading the array contents") {

            THEN("values correspond to test values") {
                int i = 1;
                while (dataProvider.next()) {
                    REQUIRE(dataProvider.getRow().size() == ADV_COLUMN_COUNT);
                    int j = 0;
                    for (const auto &value : dataProvider.getRow()) {
                        CHECK(value == advancedRecord[i][j]);
                        j++;
                    }
                    i++;
                }
            }
        }

        WHEN("restarting the reading") {

            THEN("the read contents are the same") {
                std::vector<std::vector<std::string>> toCheck;
                while (dataProvider.next()) {
                    std::vector<std::string> row;
                    for (const auto &value : dataProvider.getRow()) {
                        row.emplace_back(value);
                    }
                    toCheck.emplace_back(row);
                }

                dataProvider.first();

                int i = 0;
                while (dataProvider.next()) {
                    REQUIRE(dataProvider.getRow().size() == ADV_COLUMN_COUNT);
                    for (int j = 0; j < toCheck[i].size(); ++j) {
                        CHECK(dataProvider.getRow()[j] == toCheck[i][j]);
                    }
                    dataProvider.next();
                }
            }
        }
    }
}

SCENARIO("Reading array via iterator ArrayDataProvider", "[ArrayDataProvider]") {

    GIVEN("A simple array") {
        const std::vector<std::vector<std::string>> recordsSmall{
                {"COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"},
                {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
                {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
                {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
        };

        DataProviders::ArrayDataProvider dataProvider(recordsSmall);

        WHEN("reading the array contents") {

            THEN("values correspond to test values") {
                int recordCount = 1;
                for (const auto &value : dataProvider) {
                    REQUIRE(value.size() == SMALL_COLUMN_COUNT);
                    for (auto j = 0; j < SMALL_COLUMN_COUNT; ++j) {
                        CHECK(value[j] == recordsSmall[recordCount][j]);
                    }
                    recordCount++;
                }
                REQUIRE(recordCount-1 == SMALL_ROW_COUNT);
            }
        }

        WHEN("restarting the reading") {

            THEN("the read contents are the same") {
                std::vector<std::vector<std::string>> toCheck;
                std::copy(dataProvider.begin(),
                          dataProvider.end(),
                          std::back_inserter(toCheck));

                dataProvider.first();

                int recordCount = 1;
                for (const auto &value : dataProvider) {
                    REQUIRE(value.size() == SMALL_COLUMN_COUNT);
                    for (auto j = 0; j < toCheck[recordCount].size(); ++j) {
                        CHECK(value[j] == toCheck[recordCount][j]);
                    }
                    ++recordCount;
                }
                REQUIRE(recordCount-1 == SMALL_ROW_COUNT);
            }
        }

        WHEN("Reading column name and count"){
            THEN("Column names and count are matching"){
                REQUIRE(dataProvider.getColumnCount() == SMALL_COLUMN_COUNT);
                for(auto i =0; i < SMALL_COLUMN_COUNT; ++i){
                    REQUIRE((dataProvider.getColumnName(i)) == recordsSmall[0][i]);
                }
            }
        }
    }

    GIVEN("A bit complex array") {
        const std::vector<std::vector<std::string>> advancedRecord{
                {"A       ", "B", "C"},
                {"\"Ah, oj\"", "B", "C"},
                {"\"Ah, oj\"", "B", R"("test man"", jojo")"},
        };

        DataProviders::ArrayDataProvider dataProvider(advancedRecord);

        WHEN("reading the array contents") {

            THEN("values correspond to test values") {
                int recordCount = 0;
                for (const auto &value : dataProvider) {
                    REQUIRE(value.size() == ADV_COLUMN_COUNT);
                    for (auto j = 0; j < ADV_COLUMN_COUNT; ++j) {
                        CHECK(value[j] == advancedRecord[recordCount][j]);
                    }
                    recordCount++;
                }
                REQUIRE(recordCount-1 == ADV_ROW_COUNT);
            }
        }

        WHEN("restarting the reading") {

            THEN("the read contents are the same") {
                std::vector<std::vector<std::string>> toCheck;
                std::copy(dataProvider.begin(),
                          dataProvider.end(),
                          std::back_inserter(toCheck));

                dataProvider.first();

                int recordCount = 1;
                for (const auto &value : dataProvider) {
                    REQUIRE(value.size() == ADV_COLUMN_COUNT);
                    for (auto j = 0; j < toCheck[recordCount].size(); ++j) {
                        CHECK(value[j] == toCheck[recordCount][j]);
                    }
                    ++recordCount;
                }
                REQUIRE(recordCount-1 == ADV_ROW_COUNT);
            }
        }
    }
}
