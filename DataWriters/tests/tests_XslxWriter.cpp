//
// Created by Petr Flajsingr on 2019-01-25.
//

#include "gtest/gtest.h"
#include <XlsxWriter.h>

class XlsxWriter_tests : public ::testing::Test {
 protected:
  XlsxWriter *writer;
  std::string path = "../DataWriters/tests/out/test.xlsx";
  std::string sheetName = "test";

 public:
  XlsxWriter_tests() = default;

  void SetUp() override {
    writer = new XlsxWriter(path, sheetName);
  }

  void TearDown() override {
    delete writer;
  }

  ~XlsxWriter_tests() override = default;
};

TEST_F(XlsxWriter_tests, write_test) {
  writer->writeHeader({"Column 1", "Sloupec 2", "732864327846", "ěščšřžýžřčřč", "jes man taco man"});
  writer->writeRecord({"první", "druhý", "třetí", "čtvrtý", "něco, někdy ůůůůě+"});
  writer->writeRecord({"první", "druhý", "třetí", "čtvrtý", "něco, někdy ůůůůě+"});
  writer->writeRecord({"první", "druhý", "třetí", "čtvrtý", "něco, někdy ůůůůě+"});
  writer->writeRecord({"první", "druhý", "třetí", "čtvrtý", "něco, někdy ůůůůě+"});
  writer->writeRecord({"první", "druhý", "třetí", "čtvrtý", "něco, někdy ůůůůě+"});
}