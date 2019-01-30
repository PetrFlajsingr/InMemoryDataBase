//
// Created by Petr Flajsingr on 2019-01-29.
//

//
// Created by Petr Flajsingr on 2019-01-30.
//

#include <catch.hpp>
#include <XlsReader.h>

SCENARIO("Reading xlsx file via BaseDataProvider interface", "[XlsReader]") {

  GIVEN("A usual xlsx file") {
    const std::string columnNamesSmall[5]{
        "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
    };

    const std::string recordsSmall[3][5]{
        {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECORD15"},
        {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
        {"RECORD31", "RECORD32", "RECOR33", "RECORD34", "RECORD35"}
    };

    auto reader =
        new DataProviders::XlsReader("../DataProviders/tests/Files/simple.xlsx");

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

}

SCENARIO("Reading xlsx file via iterator", "[XlsReader]") {

  GIVEN("A simple csv file") {
    const std::string recordsSmall[3][5]{
        {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECORD15"},
        {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
        {"RECORD31", "RECORD32", "RECOR33", "RECORD34", "RECORD35"}
    };

    auto reader =
        new DataProviders::XlsReader("../DataProviders/tests/Files/simple.xlsx");

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

}