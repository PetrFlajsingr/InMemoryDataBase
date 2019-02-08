//
// Created by Petr Flajsingr on 30/08/2018.
//

#include "gtest/gtest.h"
#include "MemoryDataSet.h"
#include "ArrayDataProvider.h"

using namespace DataSets;
using namespace DataProviders;

class MemoryDataset_tests : public ::testing::Test {
 protected:
  std::vector<ValueType> types;
  static const unsigned int columnCount = 5;

  MemoryDataSet *dataset{};

  BaseDataProvider *dataProvider{};

  std::string columnNamesSmall[columnCount]{
      "0", "1", "2", "3", "4"
  };
  std::vector<std::vector<std::string>> test{{"COLUMN11", "COLUMN12", "COLUMN13", "COLUMN14", "COLUMN15"},
                                             {"COLUMN21", "COLUMN22", "COLUMN23", "COLUMN24", "COLUMN25"},
                                             {"COLUMN31", "COLUMN32", "COLUMN33", "COLUMN34", "COLUMN35"},
                                             {"COLUMN41", "COLUMN42", "COLUMN43", "COLUMN44", "COLUMN45"},
                                             {"COLUMN51", "COLUMN52", "COLUMN53", "COLUMN54", "COLUMN55"}
  };

 public:
  MemoryDataset_tests() = default;

  void SetUp() override {
    dataset = new MemoryDataSet("main");
    dataProvider = new ArrayDataProvider(test);
    for (int iter = 0; iter < dataProvider->getColumnCount(); iter++) {
      types.push_back(ValueType::String);
    }
  }

  void TearDown() override {
    types.clear();
    delete dataset;
    delete dataProvider;
  }

  ~MemoryDataset_tests() override = default;
};

 class MemoryDatasetSort_tests : public ::testing::Test {
  protected:
   std::vector<ValueType> types;

   MemoryDataSet *dataset{};

   BaseDataProvider *dataProvider{};

   std::vector<std::vector<std::string>> sortTest{{"4", "2", "3", "1", "4"},
                                                  {"2", "8", "3", "2", "1"},
                                                  {"1", "2", "3", "4", "5"},
                                                  {"1", "3", "4", "5", "6"},
                                                  {"6", "8", "3", "2", "3"},
                                                  {"1", "1", "3", "2", "6"},
                                                  {"2", "7", "5", "3", "2"},
                                                  {"9", "6", "4", "2", "1"},
                                                  {"9", "6", "4", "2", "1"},
                                                  {"6", "8", "3", "2", "1"},
                                                  {"6", "3", "3", "2", "1"},
                                                  {"2", "2", "3", "2", "1"},
   };

  protected:
   void setAsInteger() {
     for (int iter = 0; iter < dataProvider->getColumnCount(); iter++) {
       types.push_back(ValueType::Integer);
     }
   }

   void setAsDouble() {
     for (int iter = 0; iter < dataProvider->getColumnCount(); iter++) {
       types.push_back(ValueType::Double);
     }
   }

   void setAsString() {
     for (int iter = 0; iter < dataProvider->getColumnCount(); iter++) {
       types.push_back(ValueType::String);
     }
   }

   void setAsCurrency() {
     for (int iter = 0; iter < dataProvider->getColumnCount(); iter++) {
       types.push_back(ValueType::Currency);
     }
   }
  public:
   MemoryDatasetSort_tests() = default;

   void SetUp() override {
     dataset = new MemoryDataSet("main");
     dataProvider = new ArrayDataProvider(sortTest);
   }

   void TearDown() override {
     types.clear();
     delete dataset;
     delete dataProvider;
   }

   ~MemoryDatasetSort_tests() override = default;
 };

class MemoryDatasetCurrency_tests : public ::testing::Test {
 protected:
  std::vector<ValueType> types;
  std::vector<std::vector<std::string>> currencyData{{"0.00", "0.01", "0.10", "23.80", "248.99"},
                                                     {"0.80", "0.01", "0.10", "23.80", "248.99"},
                                                     {"1.00", "232.01", "0.10", "23.80", "248.99"},
                                                     {"2.00", "88.89", "0.10", "23.80", "248.99"},
                                                     {"3.00", "29999.92", "232.10", "23.80", "248.99"},
  };

  static const unsigned int columnCount = 5;

  MemoryDataSet *dataset{};

  BaseDataProvider *dataProvider{};

 public:
  MemoryDatasetCurrency_tests() = default;

  void SetUp() override {
    dataset = new MemoryDataSet("main");
    dataProvider = new ArrayDataProvider(currencyData);
    for (int iter = 0; iter < dataProvider->getColumnCount(); iter++) {
      types.push_back(ValueType::Currency);
    }
  }

  void TearDown() override {
    types.clear();
    delete dataset;
    delete dataProvider;
  }

  ~MemoryDatasetCurrency_tests() override = default;
};

TEST_F(MemoryDataset_tests, open) {
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  auto value = dataset->getFieldNames();

  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(value[i], columnNamesSmall[i]);
  }

  dataset->close();
}

TEST_F(MemoryDataset_tests, columns) {
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  auto values = dataset->getFieldNames();

  ASSERT_EQ(5, values.size());
  for (int i = 0; i < values.size(); ++i) {
    EXPECT_EQ(values[i], columnNamesSmall[i]);
  }
}

TEST_F(MemoryDataset_tests, fields) {
  std::vector<BaseField *> fieldVector{};

  unsigned int i = 0;
  for (auto iter : columnNamesSmall) {
    fieldVector.push_back(dataset->fieldByIndex(i));

    EXPECT_EQ(dataset->fieldByName(iter), fieldVector[i]);
    EXPECT_EQ(fieldVector[i]->getFieldName(), iter);
    i++;
  }

}

TEST_F(MemoryDataset_tests, read) {
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  unsigned int row = 0;
  while (dataset->next()) {
    for (int j = 0; j < 5; ++j) {
      EXPECT_EQ(dataset->fieldByIndex(j)->getAsString(), test[row][j]);
    }
    row++;
  }

  EXPECT_EQ(row, 5);
}

TEST_F(MemoryDataset_tests, move_in_dataset) {
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  std::vector<BaseField *> fields;
  for (int i = 0; i < dataset->getFieldNames().size(); ++i) {
    fields.push_back(dataset->fieldByIndex(i));
  }

  std::vector<std::string> temp;
  for (auto field : fields) {
    temp.push_back(std::string(field->getAsString()));
  }
  //
  dataset->next();

  int i = 0;
  for (auto field : fields) {
    EXPECT_NE(field->getAsString(), temp[i]);
    i++;
  }
  //
  dataset->previous();

  i = 0;
  for (auto field : fields) {
    EXPECT_EQ(field->getAsString(), temp[i]);
    i++;
  }
  //
  dataset->first();

  i = 0;
  for (auto field : fields) {
    EXPECT_EQ(field->getAsString(), temp[i]);
    i++;
  }
  //
  dataset->last();

  i = 0;
  temp.clear();
  for (auto field : fields) {
    temp.push_back(std::string(field->getAsString()));
    i++;
  }
  //
  dataset->first();
  //
  while (dataset->next()) {}

  i = 0;
  for (auto field : fields) {
    EXPECT_EQ(field->getAsString(), temp[i]);
    i++;
  }
}

TEST_F(MemoryDataset_tests, filter_equals) {
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  FilterOptions options;
  for (int i = 0; i < test[0].size(); ++i) {
    DataContainer container;
    container._string = strdup(("COLUMN" + std::to_string(i)).c_str());
    options.addOption(dataset->fieldByIndex(i),
                      {container},
                      FilterOption::Equals);
    dataset->filter(options);

    options.options.clear();
  }
  for (int i = 0; i < test.size(); ++i) {
    for (int j = 0; j < test[i].size(); ++j) {
      DataContainer container;
      container._string = strdup(("COLUMN" + std::to_string(i + 1) + std::to_string(j + 1)).c_str());
      options.addOption(dataset->fieldByIndex(j),
                        {container},
                        FilterOption::Equals);
      dataset->filter(options);

      dataset->first();
      EXPECT_EQ(dataset->fieldByIndex(j)->getAsString(), test[i][j]);

      options.options.clear();
    }
  }
}

TEST_F(MemoryDataset_tests, filter_starts_with) {
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  FilterOptions options;
  for (int i = 0; i < test[0].size(); ++i) {
    DataContainer container;
    container._string = strdup("COL");
    options.addOption(dataset->fieldByIndex(i),
                      {container},
                      FilterOption::StartsWith);
    dataset->filter(options);

    options.options.clear();

    dataset->first();

    EXPECT_EQ(dataset->fieldByIndex(i)->getAsString(), test[0][i]);
  }
}

TEST_F(MemoryDataset_tests, filter_ends_with) {
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  FilterOptions options;
  for (int i = 0; i < test[0].size(); ++i) {
    DataContainer container;
    container._string = strdup("1");
    options.addOption(dataset->fieldByIndex(i),
                      {container},
                      FilterOption::EndsWith);
    dataset->filter(options);

    options.options.clear();

    dataset->first();
    if (i == 0) {
      for (int j = 0; j < test.size(); ++j) {
        EXPECT_EQ(dataset->fieldByIndex(i)->getAsString(), test[j][i]);
        dataset->next();
      }
    } else {
    }
  }
}

TEST_F(MemoryDataset_tests, sort) {
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOptions sortOptions0;
  sortOptions0.addOption(dataset->fieldByIndex(0), SortOrder::Descending);

  //
  dataset->sort(sortOptions0);
  BaseField *col1 = dataset->fieldByIndex(0);

  auto previous = col1->getAsString();

  dataset->next();
  while (dataset->next()) {
    EXPECT_TRUE(std::strcmp(std::string(col1->getAsString()).c_str(),
                            std::string(previous).c_str()) < 0);

    previous = col1->getAsString();
  }
  //\
  //

  SortOptions sortOptions3;
  sortOptions3.addOption(dataset->fieldByIndex(3), SortOrder::Ascending);
  dataset->sort(sortOptions3);
  BaseField *col3 = dataset->fieldByIndex(3);

  previous = col3->getAsString();

  dataset->next();
  while (dataset->next()) {
    EXPECT_TRUE(std::strcmp(std::string(col3->getAsString()).c_str(),
                            std::string(previous).c_str()) > 0);

    previous = col3->getAsString();
  }
}

TEST_F(MemoryDataset_tests, append) {
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  std::vector<std::string> last;

  dataset->last();
  for (int i = 0; i < test[0].size(); ++i) {
    last.push_back(std::string(dataset->fieldByIndex(i)->getAsString()));
  }

  dataset->append();
  for (unsigned long i = 0; i < test[0].size(); ++i) {
    dataset->fieldByIndex(i)->setAsString(std::to_string(i));
  }

  for (int i = 0; i < test[0].size(); ++i) {
    EXPECT_EQ(dataset->fieldByIndex(i)->getAsString(), std::to_string(i));
  }

  dataset->previous();
  for (int i = 0; i < test[0].size(); ++i) {
    EXPECT_EQ(dataset->fieldByIndex(i)->getAsString(), last[i]);
  }

  dataset->next();
  for (int i = 0; i < test[0].size(); ++i) {
    EXPECT_EQ(dataset->fieldByIndex(i)->getAsString(), std::to_string(i));
  }
}

TEST_F(MemoryDataset_tests, append_provider) {
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  BaseDataProvider *provider = new ArrayDataProvider(test);

  ASSERT_NO_THROW(dataset->append(*provider));

  dataset->first();

  int row = 0;
  while (dataset->next()) {
    for (int i = 0; i < dataset->getFieldNames().size(); ++i) {
      EXPECT_EQ(dataset->fieldByIndex(i)->getAsString(), test[row % test.size()][i]);
    }
    row++;
  }
}

TEST_F(MemoryDatasetCurrency_tests, read) {
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  unsigned int row = 0;
  while (dataset->next()) {
    for (int j = 0; j < 5; ++j) {
      auto *field = dynamic_cast<CurrencyField *>(dataset->fieldByIndex(j));
      EXPECT_EQ(field->getAsString(),
                currencyData[row][j]);
      EXPECT_EQ(field->getAsCurrency(),
                dec::fromString<Currency>(currencyData[row][j]));
    }
    row++;
  }

  EXPECT_EQ(row, 5);
}

TEST_F(MemoryDatasetSort_tests, sortAscendingInteger) {
  setAsInteger();
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOptions options;
  for (int i = 0; i < 5;++i) {
    options.addOption(dataset->fieldByIndex(i), SortOrder::Ascending);
  }
  //

  dataset->sort(options);

  IntegerField *fields[5];
  int previous[5];

  for (int i = 0; i < 5; ++i) {
    fields[i] = dynamic_cast<IntegerField*>(dataset->fieldByIndex(i));
    previous[i] = fields[i]->getAsInteger();
  }

  dataset->next();
  int cnt = 0;
  while (dataset->next()) {
    cnt++;
    for (int i = 0; i < 5; ++i) {
      EXPECT_GE(fields[i]->getAsInteger(), previous[i]);
      if (fields[i]->getAsInteger() != previous[i]) {
        break;
      }
    }

    for (int i = 0; i < 5; ++i) {
      previous[i] = fields[i]->getAsInteger();
    }
  }
}


TEST_F(MemoryDatasetSort_tests, sortDescendingInteger) {
  setAsInteger();
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOptions options;
  for (int i = 0; i < 5;++i) {
    options.addOption(dataset->fieldByIndex(i), SortOrder::Descending);
  }
  //

  dataset->sort(options);

  IntegerField *fields[5];
  int previous[5];

  for (int i = 0; i < 5; ++i) {
    fields[i] = dynamic_cast<IntegerField*>(dataset->fieldByIndex(i));
    previous[i] = fields[i]->getAsInteger();
  }

  dataset->next();
  int cnt = 0;
  while (dataset->next()) {
    cnt++;
    for (int i = 0; i < 5; ++i) {
      EXPECT_LE(fields[i]->getAsInteger(), previous[i]);
      if (fields[i]->getAsInteger() != previous[i]) {
        break;
      }
    }

    for (int i = 0; i < 5; ++i) {
      previous[i] = fields[i]->getAsInteger();
    }
  }
}


TEST_F(MemoryDatasetSort_tests, sortCombinedInteger) {
  setAsInteger();
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOrder sortOrders[5] = {SortOrder::Ascending,
                             SortOrder::Descending,
                             SortOrder::Descending,
                             SortOrder::Ascending,
                             SortOrder::Descending};

  SortOptions options;
  for (int i = 0; i < 5;++i) {
    options.addOption(dataset->fieldByIndex(i), sortOrders[i]);
  }

  //

  dataset->sort(options);

  IntegerField *fields[5];
  int previous[5];

  for (int i = 0; i < 5; ++i) {
    fields[i] = dynamic_cast<IntegerField*>(dataset->fieldByIndex(i));
    previous[i] = fields[i]->getAsInteger();
  }

  dataset->next();
  int cnt = 0;
  while (dataset->next()) {
    cnt++;
    for (int i = 0; i < 5; ++i) {
      if (sortOrders[i] == SortOrder::Ascending) {
        EXPECT_GE(fields[i]->getAsInteger(), previous[i]);
      } else {
        EXPECT_LE(fields[i]->getAsInteger(), previous[i]);
      }
      if (fields[i]->getAsInteger() != previous[i]) {
        break;
      }
    }

    for (int i = 0; i < 5; ++i) {
      previous[i] = fields[i]->getAsInteger();
    }
  }
}

TEST_F(MemoryDatasetSort_tests, sortAscendingDouble) {
  setAsDouble();
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOptions options;
  for (int i = 0; i < 5;++i) {
    options.addOption(dataset->fieldByIndex(i), SortOrder::Ascending);
  }
  //

  dataset->sort(options);

  DoubleField *fields[5];
  double previous[5];

  for (int i = 0; i < 5; ++i) {
    fields[i] = dynamic_cast<DoubleField*>(dataset->fieldByIndex(i));
    previous[i] = fields[i]->getAsDouble();
  }

  dataset->next();
  int cnt = 0;
  while (dataset->next()) {
    cnt++;
    for (int i = 0; i < 5; ++i) {
      EXPECT_GE(fields[i]->getAsDouble(), previous[i]);
      if (fields[i]->getAsDouble() != previous[i]) {
        break;
      }
    }

    for (int i = 0; i < 5; ++i) {
      previous[i] = fields[i]->getAsDouble();
    }
  }
}


TEST_F(MemoryDatasetSort_tests, sortDescendingDouble) {
  setAsDouble();
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOptions options;
  for (int i = 0; i < 5;++i) {
    options.addOption(dataset->fieldByIndex(i), SortOrder::Descending);
  }
  //

  dataset->sort(options);

  DoubleField *fields[5];
  double previous[5];

  for (int i = 0; i < 5; ++i) {
    fields[i] = dynamic_cast<DoubleField*>(dataset->fieldByIndex(i));
    previous[i] = fields[i]->getAsDouble();
  }

  dataset->next();
  int cnt = 0;
  while (dataset->next()) {
    cnt++;
    for (int i = 0; i < 5; ++i) {
      EXPECT_LE(fields[i]->getAsDouble(), previous[i]);
      if (fields[i]->getAsDouble() != previous[i]) {
        break;
      }
    }

    for (int i = 0; i < 5; ++i) {
      previous[i] = fields[i]->getAsDouble();
    }
  }
}


TEST_F(MemoryDatasetSort_tests, sortCombinedDouble) {
  setAsDouble();
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOrder sortOrders[5] = {SortOrder::Ascending,
                             SortOrder::Descending,
                             SortOrder::Descending,
                             SortOrder::Ascending,
                             SortOrder::Descending};

  SortOptions options;
  for (int i = 0; i < 5;++i) {
    options.addOption(dataset->fieldByIndex(i), sortOrders[i]);
  }

  //

  dataset->sort(options);

  DoubleField *fields[5];
  double previous[5];

  for (int i = 0; i < 5; ++i) {
    fields[i] = dynamic_cast<DoubleField*>(dataset->fieldByIndex(i));
    previous[i] = fields[i]->getAsDouble();
  }

  dataset->next();
  int cnt = 0;
  while (dataset->next()) {
    cnt++;
    for (int i = 0; i < 5; ++i) {
      if (sortOrders[i] == SortOrder::Ascending) {
        EXPECT_GE(fields[i]->getAsDouble(), previous[i]);
      } else {
        EXPECT_LE(fields[i]->getAsDouble(), previous[i]);
      }
      if (fields[i]->getAsDouble() != previous[i]) {
        break;
      }
    }

    for (int i = 0; i < 5; ++i) {
      previous[i] = fields[i]->getAsDouble();
    }
  }
}

TEST_F(MemoryDatasetSort_tests, sortAscendingString) {
  setAsString();
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOptions options;
  for (int i = 0; i < 5;++i) {
    options.addOption(dataset->fieldByIndex(i), SortOrder::Ascending);
  }
  //

  dataset->sort(options);

  StringField *fields[5];
  std::string previous[5];

  for (int i = 0; i < 5; ++i) {
    fields[i] = dynamic_cast<StringField*>(dataset->fieldByIndex(i));
    previous[i] = fields[i]->getAsString();
  }

  dataset->next();
  int cnt = 0;
  while (dataset->next()) {
    cnt++;
    for (int i = 0; i < 5; ++i) {
      EXPECT_GE(fields[i]->getAsString(), previous[i]);
      if (fields[i]->getAsString() != previous[i]) {
        break;
      }
    }

    for (int i = 0; i < 5; ++i) {
      previous[i] = fields[i]->getAsString();
    }
  }
}


TEST_F(MemoryDatasetSort_tests, sortDescendingString) {
  setAsString();
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOptions options;
  for (int i = 0; i < 5;++i) {
    options.addOption(dataset->fieldByIndex(i), SortOrder::Descending);
  }
  //

  dataset->sort(options);

  StringField *fields[5];
  std::string previous[5];

  for (int i = 0; i < 5; ++i) {
    fields[i] = dynamic_cast<StringField*>(dataset->fieldByIndex(i));
    previous[i] = fields[i]->getAsString();
  }

  dataset->next();
  int cnt = 0;
  while (dataset->next()) {
    cnt++;
    for (int i = 0; i < 5; ++i) {
      EXPECT_LE(fields[i]->getAsString(), previous[i]);
      if (fields[i]->getAsString() != previous[i]) {
        break;
      }
    }

    for (int i = 0; i < 5; ++i) {
      previous[i] = fields[i]->getAsString();
    }
  }
}


TEST_F(MemoryDatasetSort_tests, sortCombinedString) {
  setAsString();
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOrder sortOrders[5] = {SortOrder::Ascending,
                             SortOrder::Descending,
                             SortOrder::Descending,
                             SortOrder::Ascending,
                             SortOrder::Descending};

  SortOptions options;
  for (int i = 0; i < 5;++i) {
    options.addOption(dataset->fieldByIndex(i), sortOrders[i]);
  }

  //

  dataset->sort(options);

  StringField *fields[5];
  std::string previous[5];

  for (int i = 0; i < 5; ++i) {
    fields[i] = dynamic_cast<StringField*>(dataset->fieldByIndex(i));
    previous[i] = fields[i]->getAsString();
  }

  dataset->next();
  int cnt = 0;
  while (dataset->next()) {
    cnt++;
    for (int i = 0; i < 5; ++i) {
      if (sortOrders[i] == SortOrder::Ascending) {
        EXPECT_GE(fields[i]->getAsString(), previous[i]);
      } else {
        EXPECT_LE(fields[i]->getAsString(), previous[i]);
      }
      if (fields[i]->getAsString() != previous[i]) {
        break;
      }
    }

    for (int i = 0; i < 5; ++i) {
      previous[i] = fields[i]->getAsString();
    }
  }
}


TEST_F(MemoryDatasetSort_tests, sortAscendingCurrency) {
  setAsCurrency();
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOptions options;
  for (int i = 0; i < 5;++i) {
    options.addOption(dataset->fieldByIndex(i), SortOrder::Ascending);
  }
  //

  dataset->sort(options);

  CurrencyField *fields[5];
  Currency previous[5];

  for (int i = 0; i < 5; ++i) {
    fields[i] = dynamic_cast<CurrencyField*>(dataset->fieldByIndex(i));
    previous[i] = fields[i]->getAsCurrency();
  }

  dataset->next();
  int cnt = 0;
  while (dataset->next()) {
    cnt++;
    for (int i = 0; i < 5; ++i) {
      EXPECT_GE(fields[i]->getAsCurrency(), previous[i]);
      if (fields[i]->getAsCurrency() != previous[i]) {
        break;
      }
    }

    for (int i = 0; i < 5; ++i) {
      previous[i] = fields[i]->getAsCurrency();
    }
  }
}


TEST_F(MemoryDatasetSort_tests, sortDescendingCurrency) {
  setAsCurrency();
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOptions options;
  for (int i = 0; i < 5; ++i) {
    options.addOption(dataset->fieldByIndex(i), SortOrder::Descending);
  }
  //

  dataset->sort(options);

  CurrencyField *fields[5];
  Currency previous[5];

  for (int i = 0; i < 5; ++i) {
    fields[i] = dynamic_cast<CurrencyField*>(dataset->fieldByIndex(i));
    previous[i] = fields[i]->getAsCurrency();
  }

  dataset->next();
  int cnt = 0;
  while (dataset->next()) {
    cnt++;
    for (int i = 0; i < 5; ++i) {
      EXPECT_LE(fields[i]->getAsCurrency(), previous[i]);
      if (fields[i]->getAsCurrency() != previous[i]) {
        break;
      }
    }

    for (int i = 0; i < 5; ++i) {
      previous[i] = fields[i]->getAsCurrency();
    }
  }
}


TEST_F(MemoryDatasetSort_tests, sortCombinedCurrency) {
  setAsCurrency();
  ASSERT_NO_THROW(dataset->open(*dataProvider, types));

  SortOrder sortOrders[5] = {SortOrder::Ascending,
                             SortOrder::Descending,
                             SortOrder::Descending,
                             SortOrder::Ascending,
                             SortOrder::Descending};

  SortOptions options;
  for (int i = 0; i < 5;++i) {
    options.addOption(dataset->fieldByIndex(i), sortOrders[i]);
  }

  //

  dataset->sort(options);

  CurrencyField *fields[5];
  Currency previous[5];

  for (int i = 0; i < 5; ++i) {
    fields[i] = dynamic_cast<CurrencyField*>(dataset->fieldByIndex(i));
    previous[i] = fields[i]->getAsCurrency();
  }

  dataset->next();
  int cnt = 0;
  while (dataset->next()) {
    cnt++;
    for (int i = 0; i < 5; ++i) {
      if (sortOrders[i] == SortOrder::Ascending) {
        EXPECT_GE(fields[i]->getAsCurrency(), previous[i]);
      } else {
        EXPECT_LE(fields[i]->getAsCurrency(), previous[i]);
      }
      if (fields[i]->getAsCurrency() != previous[i]) {
        break;
      }
    }

    for (int i = 0; i < 5; ++i) {
      previous[i] = fields[i]->getAsCurrency();
    }
  }
}