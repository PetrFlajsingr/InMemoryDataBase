//
// Created by Igor Frank on 03.10.19.
//

#include <CsvStreamReader.h>
#include <catch2/catch.hpp>
#include <ArrayWriter.h>
#include <csvstream.h>

#define SMALL_COLUMN_COUNT 5
#define SMALL_ROW_COUNT 3
#define ADV_COLUMN_COUNT 3
#define ADV_ROW_COUNT 2

SCENARIO("Reading csv file using via BaseDataProvider StreamReader interface", "[CsvStreamReader]"){
    GIVEN("Simple csv file"){
        const std::string columnNamesSmall[SMALL_COLUMN_COUNT]{
                "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
        };

        const std::string recordsSmall[SMALL_ROW_COUNT][SMALL_COLUMN_COUNT]{
                {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
                {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
                {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
        };

        DataProviders::CsvStreamReader csvStreamReader("../DataProviders/tests/Files/small.csv",
                                           ';');
        DataProviders::CsvStreamReader csvStreamReaderBad("../DataProviders/tests/Files/bad.csv",
                                                       ';');

        WHEN("reading the header") {

            THEN("header is the same as test value") {
                REQUIRE(csvStreamReader.getHeader().size() == SMALL_COLUMN_COUNT);
                int i = 0;
                for (const auto &value : csvStreamReader.getHeader()) {
                    CHECK(value == columnNamesSmall[i]);
                    i++;
                }
            }
        }

        WHEN("reading the file contents") {

            THEN("values correspond to test values") {
                int recordCount = 0;
                while (csvStreamReader.next()) {
                    REQUIRE(csvStreamReader.getRow().size() == SMALL_COLUMN_COUNT);
                    for (auto j = 0; j < SMALL_COLUMN_COUNT; ++j) {
                        CHECK(csvStreamReader.getRow()[j] == recordsSmall[recordCount][j]);
                    }
                    ++recordCount;
                }
                REQUIRE(recordCount == SMALL_ROW_COUNT);
            }
        }

        WHEN("restarting the reading") {

            THEN("the read contents are the same") {
                std::vector<std::vector<std::string>> toCheck;
                while (csvStreamReader.next()) {
                    std::vector<std::string> row;
                    std::copy(csvStreamReader.getRow().begin(),
                              csvStreamReader.getRow().end(),
                              std::back_inserter(row));
                    toCheck.emplace_back(row);
                }

                csvStreamReader.first();

                int recordCount = 0;
                while (csvStreamReader.next()) {
                    REQUIRE(csvStreamReader.getRow().size() == SMALL_COLUMN_COUNT);
                    for (gsl::index j = 0; j < static_cast<gsl::index>(toCheck[recordCount].size()); ++j) {
                        CHECK(csvStreamReader.getRow()[j] == toCheck[recordCount][j]);
                    }
                    recordCount++;
                }
                REQUIRE(recordCount == SMALL_ROW_COUNT);
            }
        }

        WHEN("Reading column name and count"){
            THEN("Column names and count are matching"){
                REQUIRE(csvStreamReader.getColumnCount() == SMALL_COLUMN_COUNT);
                for(auto i =0; i < SMALL_COLUMN_COUNT; ++i){
                    REQUIRE((csvStreamReader.getColumnName(i)) == columnNamesSmall[i]);
                }
            }
        }

        WHEN("Reading column that doesnt match with header"){
            THEN("Throw exception"){
                    REQUIRE_THROWS(csvStreamReaderBad.next());
            }
        }
    }

    GIVEN("A bit complex csv") {
        const std::string advancedHeader[ADV_COLUMN_COUNT]{
                "A       ", "B", "C"
        };

        const std::string advancedTest[ADV_ROW_COUNT][ADV_COLUMN_COUNT]{
                {"\"Ah, oj\"", "B", "C"},
                {"\"Ah, oj\"", "B", R"("test man"", jojo")"},
        };

        DataProviders::CsvStreamReader csvStreamReader("../DataProviders/tests/Files/adv.csv",
                                           ',');

        WHEN("reading the header") {

            THEN("header is the same as test value") {
                REQUIRE(csvStreamReader.getHeader().size() == ADV_COLUMN_COUNT);
                int i = 0;
                for (const auto &value : csvStreamReader.getHeader()) {
                    CHECK(value == advancedHeader[i]);
                    i++;
                }
            }
        }

        WHEN("reading the file contents") {

            THEN("values correspond to test values") {
                int i = 0;
                while (csvStreamReader.next()) {
                    REQUIRE(csvStreamReader.getRow().size() == ADV_COLUMN_COUNT);
                    int j = 0;
                    for (const auto &value : csvStreamReader.getRow()) {
                        CHECK(value == advancedTest[i][j]);
                        j++;
                    }
                    i++;
                }
            }
        }

        WHEN("restarting the reading") {

            THEN("the read contents are the same") {
                std::vector<std::vector<std::string>> toCheck;
                while (csvStreamReader.next()) {
                    std::vector<std::string> row;
                    for (const auto &value : csvStreamReader.getRow()) {
                        row.emplace_back(value);
                    }
                    toCheck.emplace_back(row);
                }

                csvStreamReader.first();

                int i = 0;
                while (csvStreamReader.next()) {
                    REQUIRE(csvStreamReader.getRow().size() == ADV_COLUMN_COUNT);
                    for (gsl::index j = 0; j < static_cast<gsl::index>(toCheck[i].size()); ++j) {
                        CHECK(csvStreamReader.getRow()[j] == toCheck[i][j]);
                    }
                    csvStreamReader.next();
                }
            }
        }
    }
}

SCENARIO("Reading csv file via iterator StreamReader", "[CsvStreamReader]") {

    GIVEN("A simple csv file") {
        const std::string columnNamesSmall[SMALL_COLUMN_COUNT]{
                "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
        };

        const std::string recordsSmall[SMALL_ROW_COUNT][SMALL_COLUMN_COUNT]{
                {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
                {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
                {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
        };

        DataProviders::CsvStreamReader reader("../DataProviders/tests/Files/small.csv",
                                        ';');

        WHEN("reading the file contents") {

            THEN("values correspond to test values") {
                int recordCount = 0;
                for (const auto &value : reader) {
                    REQUIRE(value.size() == SMALL_COLUMN_COUNT);
                    for (auto j = 0; j < SMALL_COLUMN_COUNT; ++j) {
                        CHECK(value[j] == recordsSmall[recordCount][j]);
                    }
                    recordCount++;
                }
                REQUIRE(recordCount == SMALL_ROW_COUNT);
            }
        }

        WHEN("restarting the reading") {

            THEN("the read contents are the same") {
                std::vector<std::vector<std::string>> toCheck;
                std::copy(reader.begin(),
                          reader.end(),
                          std::back_inserter(toCheck));

                reader.first();

                int recordCount = 0;
                for (const auto &value : reader) {
                    REQUIRE(value.size() == SMALL_COLUMN_COUNT);
                    for (gsl::index j = 0; j < static_cast<gsl::index>(toCheck[recordCount].size()); ++j) {
                        CHECK(value[j] == toCheck[recordCount][j]);
                    }
                    ++recordCount;
                }
                REQUIRE(recordCount == SMALL_ROW_COUNT);
            }
        }

        WHEN("Reading column name and count"){
            THEN("Column names and count are matching"){
                REQUIRE(reader.getColumnCount() == SMALL_COLUMN_COUNT);
                for(auto i =0; i < SMALL_COLUMN_COUNT; ++i){
                    REQUIRE((reader.getColumnName(i)) == columnNamesSmall[i]);
                }
            }
        }
    }

    GIVEN("A bit complex csv") {
        const std::string advancedTest[ADV_ROW_COUNT][ADV_COLUMN_COUNT]{
                {"\"Ah, oj\"", "B", "C"},
                {"\"Ah, oj\"", "B", R"("test man"", jojo")"},
        };

        DataProviders::CsvStreamReader csvStreamReader("../DataProviders/tests/Files/adv.csv",
                                           ',');

        WHEN("reading the file contents") {

            THEN("values correspond to test values") {
                int recordCount = 0;
                for (const auto &value : csvStreamReader) {
                    REQUIRE(value.size() == ADV_COLUMN_COUNT);
                    for (auto j = 0; j < ADV_COLUMN_COUNT; ++j) {
                        CHECK(value[j] == advancedTest[recordCount][j]);
                    }
                    recordCount++;
                }
                REQUIRE(recordCount == ADV_ROW_COUNT);
            }
        }

        WHEN("restarting the reading") {

            THEN("the read contents are the same") {
                std::vector<std::vector<std::string>> toCheck;
                std::copy(csvStreamReader.begin(),
                          csvStreamReader.end(),
                          std::back_inserter(toCheck));

                csvStreamReader.first();

                int recordCount = 0;
                for (const auto &value : csvStreamReader) {
                    REQUIRE(value.size() == ADV_COLUMN_COUNT);
                    for (gsl::index j = 0; j < static_cast<gsl::index>(toCheck[recordCount].size()); ++j) {
                        CHECK(value[j] == toCheck[recordCount][j]);
                    }
                    ++recordCount;
                }
                REQUIRE(recordCount == ADV_ROW_COUNT);
            }
        }
    }
}

SCENARIO(
        "Writing parsed csv directly to DataWriter using BaseDataProvider StreamReader interface",
        "[CsvStreamReader][integration]") {

    GIVEN("A simple csv file") {
        const std::string columnNamesSmall[SMALL_COLUMN_COUNT]{
                "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
        };

        const std::string recordsSmall[SMALL_ROW_COUNT][SMALL_COLUMN_COUNT]{
                {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
                {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
                {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
        };

        DataProviders::CsvStreamReader csvStreamReader("../DataProviders/tests/Files/small.csv",
                                           ';');

        DataWriters::ArrayWriter writer;
        WHEN("reading the content") {

            THEN("data output by CsvWriter is the same as test values") {
                REQUIRE(csvStreamReader.getHeader().size() == SMALL_COLUMN_COUNT);
                auto header = csvStreamReader.getHeader();
                writer.writeHeader(header);

                for (auto i = 0; i < SMALL_COLUMN_COUNT; ++i) {
                    CHECK(writer.getArray()[0][i] == columnNamesSmall[i]);
                }

                while (csvStreamReader.next()) {
                    writer.writeRecord(csvStreamReader.getRow());
                }

                for (auto i = 0; i < SMALL_ROW_COUNT; ++i) {
                    for (auto j = 0; j < SMALL_COLUMN_COUNT; ++j) {
                        CHECK(writer.getArray()[i + 1][j] == recordsSmall[i][j]);
                    }
                }
            }
        }
    }
}

SCENARIO("Writing parsed csv directly to DataWriter using StreamReader iterator",
         "[CsvStreamReader][integration]") {

    GIVEN("A simple csv file") {
        const std::string columnNamesSmall[SMALL_COLUMN_COUNT]{
                "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
        };

        const std::string recordsSmall[SMALL_ROW_COUNT][SMALL_COLUMN_COUNT]{
                {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
                {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
                {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
        };

        DataProviders::CsvStreamReader csvStreamReader("../DataProviders/tests/Files/small.csv",
                                                       ';');

        DataWriters::ArrayWriter writer;
        WHEN("reading the content") {

            THEN("data output by CsvWriter is the same as test values") {
                REQUIRE(csvStreamReader.getHeader().size() == SMALL_COLUMN_COUNT);
                auto header = csvStreamReader.getHeader();
                writer.writeHeader(header);

                for (auto i = 0; i < SMALL_COLUMN_COUNT; ++i) {
                    CHECK(writer.getArray()[0][i] == columnNamesSmall[i]);
                }

                std::copy(csvStreamReader.begin(), csvStreamReader.end(), writer.begin());

                for (auto i = 0; i < SMALL_ROW_COUNT; ++i) {
                    for (auto j = 0; j < SMALL_COLUMN_COUNT; ++j) {
                        CHECK(writer.getArray()[i + 1][j] == recordsSmall[i][j]);
                    }
                }
            }
        }
    }
}
