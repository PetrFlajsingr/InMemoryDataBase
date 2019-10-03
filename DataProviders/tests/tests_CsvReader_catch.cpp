//
// Created by Petr Flajsingr on 2019-01-30.
//

#include <Catch2Define.h>
#include <CsvReader.h>
#include <ArrayWriter.h>
#include <catch2/catch.hpp>

#define SMALL_COLUMN_COUNT 5
#define SMALL_ROW_COUNT 3
#define ADV_COLUMN_COUNT 3
#define ADV_ROW_COUNT 2

SCENARIO("Reading csv file via BaseDataProvider interface", "[CsvReader]") {

  GIVEN("A simple csv file") {
    const std::string columnNamesSmall[SMALL_COLUMN_COUNT]{
        "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
    };

    const std::string recordsSmall[SMALL_ROW_COUNT][SMALL_COLUMN_COUNT]{
        {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
        {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
        {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
    };

    DataProviders::CsvReader csvReader("../DataProviders/tests/Files/small.csv",
                                       ";");

    WHEN("reading the header") {

      THEN("header is the same as test value") {
        REQUIRE(csvReader.getHeader().size() == SMALL_COLUMN_COUNT);
        int i = 0;
        for (const auto &value : csvReader.getHeader()) {
          CHECK(value == columnNamesSmall[i]);
          i++;
        }
      }
    }

    WHEN("reading the file contents") {

      THEN("values correspond to test values") {
        int recordCount = 0;
        while (csvReader.next()) {
          REQUIRE(csvReader.getRow().size() == SMALL_COLUMN_COUNT);
          for (auto j = 0; j < SMALL_COLUMN_COUNT; ++j) {
            CHECK(csvReader.getRow()[j] == recordsSmall[recordCount][j]);
          }
          ++recordCount;
        }
        REQUIRE(recordCount == SMALL_ROW_COUNT);
      }
    }

    WHEN("restarting the reading") {

      THEN("the read contents are the same") {
        std::vector<std::vector<std::string>> toCheck;
        while (csvReader.next()) {
          std::vector<std::string> row;
          std::copy(csvReader.getRow().begin(),
                    csvReader.getRow().end(),
                    std::back_inserter(row));
          toCheck.emplace_back(row);
        }

        csvReader.first();

        int recordCount = 0;
        while (csvReader.next()) {
          REQUIRE(csvReader.getRow().size() == SMALL_COLUMN_COUNT);
          for (int j = 0; j < toCheck[recordCount].size(); ++j) {
            CHECK(csvReader.getRow()[j] == toCheck[recordCount][j]);
          }
          recordCount++;
        }
        REQUIRE(recordCount == SMALL_ROW_COUNT);
      }
    }

    WHEN("Reading column name and count"){
        THEN("Column names and count are matching"){
            REQUIRE(csvReader.getColumnCount() == SMALL_COLUMN_COUNT);
            for(auto i =0; i < SMALL_COLUMN_COUNT; ++i){
             REQUIRE((csvReader.getColumnName(i)) == columnNamesSmall[i]);
            }
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

    DataProviders::CsvReader csvReader("../DataProviders/tests/Files/adv.csv",
                                       ",");

    WHEN("reading the header") {

      THEN("header is the same as test value") {
        REQUIRE(csvReader.getHeader().size() == ADV_COLUMN_COUNT);
        int i = 0;
        for (const auto &value : csvReader.getHeader()) {
          CHECK(value == advancedHeader[i]);
          i++;
        }
      }
    }

    WHEN("reading the file contents") {

      THEN("values correspond to test values") {
        int i = 0;
        while (csvReader.next()) {
          REQUIRE(csvReader.getRow().size() == ADV_COLUMN_COUNT);
          int j = 0;
          for (const auto &value : csvReader.getRow()) {
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
        while (csvReader.next()) {
          std::vector<std::string> row;
          for (const auto &value : csvReader.getRow()) {
            row.emplace_back(value);
          }
          toCheck.emplace_back(row);
        }

        csvReader.first();

        int i = 0;
        while (csvReader.next()) {
          REQUIRE(csvReader.getRow().size() == ADV_COLUMN_COUNT);
          for (int j = 0; j < toCheck[i].size(); ++j) {
            CHECK(csvReader.getRow()[j] == toCheck[i][j]);
          }
          csvReader.next();
        }
      }
    }
  }
}

SCENARIO("Reading csv file via iterator", "[CsvReader]") {

  GIVEN("A simple csv file") {
      const std::string columnNamesSmall[SMALL_COLUMN_COUNT]{
              "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
      };

      const std::string recordsSmall[SMALL_ROW_COUNT][SMALL_COLUMN_COUNT]{
        {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
        {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
        {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
    };

    DataProviders::CsvReader reader("../DataProviders/tests/Files/small.csv",
                                    ";");

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
          for (auto j = 0; j < toCheck[recordCount].size(); ++j) {
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

    DataProviders::CsvReader csvReader("../DataProviders/tests/Files/adv.csv",
                                       ",");

    WHEN("reading the file contents") {

      THEN("values correspond to test values") {
        int recordCount = 0;
        for (const auto &value : csvReader) {
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
        std::copy(csvReader.begin(),
                  csvReader.end(),
                  std::back_inserter(toCheck));

        csvReader.first();

        int recordCount = 0;
        for (const auto &value : csvReader) {
          REQUIRE(value.size() == ADV_COLUMN_COUNT);
          for (auto j = 0; j < toCheck[recordCount].size(); ++j) {
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
    "Writing parsed csv directly to DataWriter using BaseDataProvider interface",
    "[CsvReader][integration]") {

  GIVEN("A simple csv file") {
    const std::string columnNamesSmall[SMALL_COLUMN_COUNT]{
        "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
    };

    const std::string recordsSmall[SMALL_ROW_COUNT][SMALL_COLUMN_COUNT]{
        {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
        {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
        {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
    };

    DataProviders::CsvReader csvReader("../DataProviders/tests/Files/small.csv",
                                       ";");

    DataWriters::ArrayWriter writer;
    WHEN("reading the content") {

      THEN("data output by CsvWriter is the same as test values") {
        REQUIRE(csvReader.getHeader().size() == SMALL_COLUMN_COUNT);
        auto header = csvReader.getHeader();
        writer.writeHeader(header);

        for (auto i = 0; i < SMALL_COLUMN_COUNT; ++i) {
          CHECK(writer.getArray()[0][i] == columnNamesSmall[i]);
        }

        while (csvReader.next()) {
          writer.writeRecord(csvReader.getRow());
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

SCENARIO("Writing parsed csv directly to DataWriter using iterator",
         "[CsvReader][integration]") {

  GIVEN("A simple csv file") {
    const std::string columnNamesSmall[SMALL_COLUMN_COUNT]{
        "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
    };

    const std::string recordsSmall[SMALL_ROW_COUNT][SMALL_COLUMN_COUNT]{
        {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
        {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
        {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
    };

    DataProviders::CsvReader csvReader("../DataProviders/tests/Files/small.csv",
                                       ";");

    DataWriters::ArrayWriter writer;
    WHEN("reading the content") {

      THEN("data output by CsvWriter is the same as test values") {
        REQUIRE(csvReader.getHeader().size() == SMALL_COLUMN_COUNT);
        auto header = csvReader.getHeader();
        writer.writeHeader(header);

        for (auto i = 0; i < SMALL_COLUMN_COUNT; ++i) {
          CHECK(writer.getArray()[0][i] == columnNamesSmall[i]);
        }

        std::copy(csvReader.begin(), csvReader.end(), writer.begin());

        for (auto i = 0; i < SMALL_ROW_COUNT; ++i) {
          for (auto j = 0; j < SMALL_COLUMN_COUNT; ++j) {
            CHECK(writer.getArray()[i + 1][j] == recordsSmall[i][j]);
          }
        }
      }
    }
  }
}