//
// Created by Petr Flajsingr on 2019-01-29.
//

#include <catch2/catch.hpp>
#include <XlsReader.h>
#include <ArrayWriter.h>

#define COLUMN_COUNT 5
#define ROW_COUNT 3

SCENARIO("Reading xlsx file via BaseDataProvider interface", "[XlsReader]") {

  GIVEN("A usual xlsx file") {
    const std::string columnNamesSmall[COLUMN_COUNT]{
        "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
    };

    const std::string recordsSmall[ROW_COUNT][COLUMN_COUNT]{
        {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECORD15"},
        {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
        {"RECORD31", "RECORD32", "RECOR33", "RECORD34", "RECORD35"}
    };

    DataProviders::XlsReader
        xlsReader("../DataProviders/tests/Files/simple.xlsx");

    WHEN("reading the header") {

      THEN("header is the same as test value") {
        REQUIRE(xlsReader.getHeader().size() == COLUMN_COUNT);
        int i = 0;
        for (const auto &value : xlsReader.getHeader()) {
          REQUIRE(value == columnNamesSmall[i]);
          i++;
        }
      }
    }

    WHEN("reading the file contents") {

      THEN("values correspond to test values") {
        int recordCount = 0;
        while (xlsReader.next()) {
          REQUIRE(xlsReader.getRow().size() == COLUMN_COUNT);
          for (auto j = 0; j < COLUMN_COUNT; ++j) {
            REQUIRE(xlsReader.getRow()[j] == recordsSmall[recordCount][j]);
            j++;
          }
          recordCount++;
        }
        REQUIRE(recordCount == ROW_COUNT);
      }
    }

    WHEN("restarting the reading") {

      THEN("the read contents are the same") {
        std::vector<std::vector<std::string>> toCheck;
        while (xlsReader.next()) {
          std::vector<std::string> row;
          std::copy(xlsReader.getRow().begin(),
                    xlsReader.getRow().end(),
                    std::back_inserter(row));
          toCheck.emplace_back(row);
        }

        xlsReader.first();

        int i = 0;
        while (xlsReader.next()) {
          for (int j = 0; j < toCheck[i].size(); ++j) {
            REQUIRE(xlsReader.getRow()[j] == toCheck[i][j]);
          }
          i++;
        }
      }
    }
  }

}

SCENARIO("Reading xlsx file via iterator", "[XlsReader]") {

  GIVEN("A usual xlsx file") {
    const std::string recordsSmall[ROW_COUNT][COLUMN_COUNT]{
        {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECORD15"},
        {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
        {"RECORD31", "RECORD32", "RECOR33", "RECORD34", "RECORD35"}
    };

    DataProviders::XlsReader
        xlsReader("../DataProviders/tests/Files/simple.xlsx");

    WHEN("reading the file contents") {

      THEN("values correspond to test values") {
        int recordCount = 0;
        for (const auto &value : xlsReader) {
          REQUIRE(value.size() == COLUMN_COUNT);
          int j = 0;
          for (auto j = 0; j < COLUMN_COUNT; ++j) {
            REQUIRE(value[j] == recordsSmall[recordCount][j]);
          }
          recordCount++;
        }
        REQUIRE(recordCount == ROW_COUNT);
      }
    }

    WHEN("restarting the reading") {

      THEN("the read contents are the same") {
        std::vector<std::vector<std::string>> toCheck;
        std::copy(xlsReader.begin(),
                  xlsReader.end(),
                  std::back_inserter(toCheck));

        xlsReader.first();

        int i = 0;
        for (const auto &value : xlsReader) {
          for (auto j = 0; j < toCheck[i].size(); ++j) {
            REQUIRE(value[j] == toCheck[i][j]);
          }
          ++i;
        }
      }
    }
  }
}

SCENARIO(
    "Writing parsed xlsx directly to DataWriter using BaseDataProvider interface",
    "[XlsReader][integration]") {

  GIVEN("A usual xlsx file") {
    const std::string columnNamesSmall[COLUMN_COUNT]{
        "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
    };

    const std::string recordsSmall[ROW_COUNT][COLUMN_COUNT]{
        {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECORD15"},
        {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
        {"RECORD31", "RECORD32", "RECOR33", "RECORD34", "RECORD35"}
    };

    DataProviders::XlsReader
        xlsReader("../DataProviders/tests/Files/simple.xlsx");

    DataWriters::ArrayWriter writer;
    WHEN("reading the content") {

      THEN("data output by CsvWriter is the same as test values") {
        REQUIRE(xlsReader.getHeader().size() == COLUMN_COUNT);
        auto header = xlsReader.getHeader();
        writer.writeHeader(header);

        for (auto i = 0; i < COLUMN_COUNT; ++i) {
          REQUIRE(writer.getArray()[0][i] == columnNamesSmall[i]);
        }

        while (xlsReader.next()) {
          writer.writeRecord(xlsReader.getRow());
        }

        for (auto i = 0; i < ROW_COUNT; ++i) {
          for (auto j = 0; j < COLUMN_COUNT; ++j) {
            REQUIRE(writer.getArray()[i + 1][j] == recordsSmall[i][j]);
          }
        }
      }
    }
  }
}

SCENARIO("Writing parsed xlsx directly to DataWriter using iterator",
         "[XlsReader][integration]") {

  GIVEN("A usual xlsx file") {
    const std::string columnNamesSmall[COLUMN_COUNT]{
        "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
    };

    const std::string recordsSmall[ROW_COUNT][COLUMN_COUNT]{
        {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECORD15"},
        {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
        {"RECORD31", "RECORD32", "RECOR33", "RECORD34", "RECORD35"}
    };

    DataProviders::XlsReader
        xlsReader("../DataProviders/tests/Files/simple.xlsx");

    DataWriters::ArrayWriter writer;
    WHEN("reading the content") {

      THEN("data output by CsvWriter is the same as test values") {
        REQUIRE(xlsReader.getHeader().size() == COLUMN_COUNT);
        auto header = xlsReader.getHeader();
        writer.writeHeader(header);

        for (auto i = 0; i < COLUMN_COUNT; ++i) {
          REQUIRE(writer.getArray()[0][i] == columnNamesSmall[i]);
        }

        std::copy(xlsReader.begin(), xlsReader.end(), writer.begin());

        for (auto i = 0; i < ROW_COUNT; ++i) {
          for (auto j = 0; j < COLUMN_COUNT; ++j) {
            REQUIRE(writer.getArray()[i + 1][j] == recordsSmall[i][j]);
          }
        }
      }
    }
  }
}