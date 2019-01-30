//
// Created by Petr Flajsingr on 30/08/2018.
//

#include <gtest/gtest.h>
#include "CsvReader.h"

class CsvReader_tests : public ::testing::Test {
 protected:
  DataProviders::CsvReader *reader = nullptr;

  std::string columnNamesSmall[5]{
      "COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"
  };

  std::string recordsSmall[3][5]{
      {"RECORD11", "RECORD12", "RECORD13", "RECORD14", "RECOR15"},
      {"RECORD21", "RECORD22", "RECORD23", "RECORD24", "RECORD25"},
      {"RECORD31", "RECORD32", "RECORD33", "RECORD34", "RECORD35"}
  };

  std::string advancedTest[3][3]{
      {"\"Ah, oj\"", "B", "C"},
      {"\"Ah, oj\"", "B", R"("test man"", jojo")"},
  };

  void prepareReaderSmall();

  void prepareReaderAdv();

 public:
  CsvReader_tests() = default;

  void SetUp() override {

  }

  void TearDown() override {
    delete reader;
  }

  ~CsvReader_tests() override = default;
};

void CsvReader_tests::prepareReaderSmall() {
  reader =
      new DataProviders::CsvReader("../DataProviders/tests/Files/small.csv",
                                   ";");
}

void CsvReader_tests::prepareReaderAdv() {
  reader =
      new DataProviders::CsvReader("../DataProviders/tests/Files/adv.csv", ",");
}

TEST_F (CsvReader_tests, columns_small) {
  prepareReaderSmall();

  ASSERT_EQ(reader->getColumnCount(), 5);
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(reader->getHeader()[i], columnNamesSmall[i]);
    EXPECT_EQ(reader->getColumn(i), columnNamesSmall[i]);
  }
}

TEST_F(CsvReader_tests, records_small) {
  prepareReaderSmall();

  int x = 0;
  while (!reader->eof()) {
    auto value = reader->getRow();
    EXPECT_EQ(reader->getCurrentRecordNumber(), x);
    for (int i = 0; i < 3; ++i) {
      EXPECT_EQ(value[i], recordsSmall[x][i]);
    }
    x++;
    reader->next();
    EXPECT_EQ(reader->getCurrentRecordNumber(), x);
  }
}

TEST_F(CsvReader_tests, advanced) {
  prepareReaderAdv();

  int x = 0;
  while (!reader->eof()) {
    auto value = reader->getRow();
    EXPECT_EQ(reader->getCurrentRecordNumber(), x);
    for (int i = 0; i < 3; ++i) {
      EXPECT_EQ(value[i], advancedTest[x][i]);
    }
    x++;
    reader->next();
    EXPECT_EQ(reader->getCurrentRecordNumber(), x);
  }
}

TEST_F(CsvReader_tests, first_small) {
  prepareReaderSmall();
  while (!reader->eof()) {
    reader->next();
  }

  reader->first();

  auto value = reader->getRow();
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(value[i], recordsSmall[0][i]);
  }

}

TEST_F(CsvReader_tests, iterator) {
  prepareReaderSmall();
  int testIter = 0;
  for (auto row : *reader) {
    for (int i = 0; i < 5; ++i) {
      EXPECT_EQ(row[i], recordsSmall[testIter][i]);
    }
    testIter++;
  }
}