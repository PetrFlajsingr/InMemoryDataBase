//
// Created by Petr Flajsingr on 2019-01-30.
//

#include <catch.hpp>
#include <CsvReader.h>

SCENARIO("Using via BaseDataProvider interface", "[CsvReader]") {

  GIVEN("A simple csv file") {
    const std::string columnNamesSmall[5]{
        "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
    };

    const std::string recordsSmall[3][5]{
        {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
        {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
        {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
    };

    auto reader =
        new DataProviders::CsvReader("../DataProviders/tests/Files/small.csv",
                                     ";");

    WHEN("reading the header") {

      THEN("header is the same as test value") {
        REQUIRE(reader->getHeader().size() == 5);
        int i = 0;
        for (const auto &value : reader->getHeader()) {
          REQUIRE(value == columnNamesSmall[i]);
          i++;
        }
      }
    }

    WHEN("reading the file contents") {

      THEN("values correspond to test values") {
        int i = 0;
        while (!reader->eof()) {
          REQUIRE(reader->getRow().size() == 5);
          int j = 0;
          for (const auto &value : reader->getRow()) {
            REQUIRE(value == recordsSmall[i][j]);
            j++;
          }
          i++;
          reader->next();
        }
      }
    }

    WHEN("restarting the reading") {

      THEN("the read contents are the same") {
        std::vector<std::vector<std::string>> toCheck;
        while (!reader->eof()) {
          std::vector<std::string> row;
          std::copy(reader->getRow().begin(),
                    reader->getRow().end(),
                    std::back_inserter(row));
          toCheck.emplace_back(row);
          reader->next();
        }

        reader->first();

        int i = 0;
        while (!reader->eof()) {
          for (int j = 0; j < toCheck[i].size(); ++j) {
            REQUIRE(reader->getRow()[j] == toCheck[i][j]);
          }
          reader->next();
          i++;
        }
      }
    }
  }

  GIVEN("A bit complex csv") {
    const std::string advancedHeader[3]{
        "A       ", "B", "C"
    };

    const std::string advancedTest[3][3]{
        {"\"Ah, oj\"", "B", "C"},
        {"\"Ah, oj\"", "B", R"("test man"", jojo")"},
    };

    auto reader =
        new DataProviders::CsvReader("../DataProviders/tests/Files/adv.csv",
                                     ",");

    WHEN("reading the header") {

      THEN("header is the same as test value") {
        REQUIRE(reader->getHeader().size() == 3);
        int i = 0;
        for (const auto &value : reader->getHeader()) {
          REQUIRE(value == advancedHeader[i]);
          i++;
        }
      }
    }

    WHEN("reading the file contents") {

      THEN("values correspond to test values") {
        int i = 0;
        while (!reader->eof()) {
          REQUIRE(reader->getRow().size() == 3);
          int j = 0;
          for (const auto &value : reader->getRow()) {
            REQUIRE(value == advancedTest[i][j]);
            j++;
          }
          i++;
          reader->next();
        }
      }
    }

    WHEN("restarting the reading") {

      THEN("the read contents are the same") {
        std::vector<std::vector<std::string>> toCheck;
        while (!reader->eof()) {
          std::vector<std::string> row;
          for (const auto &value : reader->getRow()) {
            row.emplace_back(value);
          }
          toCheck.emplace_back(row);
          reader->next();
        }

        reader->first();

        int i = 0;
        while (!reader->eof()) {
          for (int j = 0; j < toCheck[i].size(); ++j) {
            REQUIRE(reader->getRow()[j] == toCheck[i][j]);
          }
          reader->next();
          i++;
        }
      }
    }
  }
}

SCENARIO("Using via iterator", "[CsvReader]") {

  GIVEN("A simple csv file") {
    const std::string recordsSmall[3][5]{
        {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
        {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
        {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
    };

    auto reader =
        new DataProviders::CsvReader("../DataProviders/tests/Files/small.csv",
                                     ";");

    WHEN("reading the file contents") {

      THEN("values correspond to test values") {
        int i = 0;
        for (const auto &value : *reader) {
          REQUIRE(reader->getRow().size() == 5);
          int j = 0;
          for (const auto &record : value) {
            REQUIRE(record == recordsSmall[i][j]);
            j++;
          }
          i++;
        }
      }
    }

    WHEN("restarting the reading") {

      THEN("the read contents are the same") {
        std::vector<std::vector<std::string>> toCheck;
        std::copy(reader->begin(),
                  reader->end(),
                  std::back_inserter(toCheck));

        reader->first();

        int i = 0;
        for (const auto &value : *reader) {
          for (auto j = 0; j < toCheck[i].size(); ++j) {
            REQUIRE(value[j] == toCheck[i][j]);
          }

          ++i;
        }
      }
    }
  }

  GIVEN("A bit complex csv") {
    const std::string advancedTest[3][3]{
        {"\"Ah, oj\"", "B", "C"},
        {"\"Ah, oj\"", "B", R"("test man"", jojo")"},
    };

    auto reader =
        new DataProviders::CsvReader("../DataProviders/tests/Files/adv.csv",
                                     ",");

    WHEN("reading the file contents") {

      THEN("values correspond to test values") {
        int i = 0;
        for (const auto &value : *reader) {
          REQUIRE(reader->getRow().size() == 3);
          int j = 0;
          for (auto j = 0; j < 3; ++j) {
            REQUIRE(value[j] == advancedTest[i][j]);
          }
          i++;
        }
      }
    }

    WHEN("restarting the reading") {

      THEN("the read contents are the same") {
        std::vector<std::vector<std::string>> toCheck;
        std::copy(reader->begin(),
                  reader->end(),
                  std::back_inserter(toCheck));

        reader->first();

        int i = 0;
        for (const auto &value : *reader) {
          for (auto j = 0; j < toCheck[i].size(); ++j) {
            REQUIRE(value[j] == toCheck[i][j]);
          }
          ++i;
        }
      }
    }
  }
}