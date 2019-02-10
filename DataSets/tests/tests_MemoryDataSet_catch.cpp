//
// Created by Petr Flajsingr on 2019-02-02.
//

#include <catch.hpp>
#include <ArrayDataProvider.h>
#include <MemoryDataSet.h>
#include <MemoryViewDataSet.h>

#define DATA_COUNT 10000
#define COL_COUNT 5

SCENARIO("Using MemoryDataSet with DataProvider", "[MemoryDataSet]") {
  GIVEN("Provider with 10 000 rows of data, string & integer (+double&currency)") {
    std::vector<std::vector<std::string>> data;
    data.reserve(DATA_COUNT);
    std::vector<std::string> row;
    row.reserve(COL_COUNT);
    int sortColVal = 0;
    int sortColVal2 = 0;
    for (gsl::index i = 0; i < DATA_COUNT; ++i) {
      auto firstPart = std::to_string(i);
      for (gsl::index j = 0; j < COL_COUNT; ++j) {
        if (j == 2) {
          row.emplace_back(std::to_string(sortColVal));
        } else if (j == 4) {
          row.emplace_back(std::to_string(sortColVal2));
        } else {
          row.emplace_back(firstPart + "_" + std::to_string(j));
        }
      }
      if ((i & 0xF) == 0) {
        sortColVal++;
      }
      if ((i & 0x7) == 0) {
        sortColVal2++;
      }
      data.emplace_back(row);
      row.clear();
    }

    DataProviders::ArrayDataProvider provider(data);

    WHEN("Reading the data from provider, string values") {
      DataSets::MemoryDataSet dataSet("test_dataset");
      auto fieldTypes = {
          ValueType::String,
          ValueType::String,
          ValueType::String,
          ValueType::String,
          ValueType::String,
      };
      dataSet.open(provider, fieldTypes);

      THEN("The data inside data set correspond to data read") {
        auto fields = dataSet.getFields();
        REQUIRE(fields.size() == COL_COUNT);

        int rowCount = 0;
        while (dataSet.next()) {
          for (auto field : fields) {
            CHECK(field->getAsString() == data[rowCount][field->getIndex()]);
          }
          rowCount++;
        }
        REQUIRE(rowCount == DATA_COUNT);

        rowCount--;
        while (dataSet.previous()) {
          auto wat = data[rowCount][0];
          for (auto field : fields) {
            CHECK(field->getAsString() == data[rowCount][field->getIndex()]);
          }
          auto wat2 = data[rowCount][0];
          rowCount--;
        }
        REQUIRE(rowCount == -1);
      }

      THEN("The data can be sorted") {
        DataSets::SortOptions sortOptions;
        sortOptions.addOption(dataSet.fieldByIndex(0), SortOrder::Descending);
        dataSet.sort(sortOptions);
        auto field0 = dataSet.fieldByIndex(0);
        // "zzz" since there are only string numbers in test data
        std::string lastVal = "zzzz";
        while (dataSet.next()) {
          REQUIRE(lastVal >= field0->getAsString());
          lastVal = field0->getAsString();
        }

        DataSets::SortOptions sortOptions2;
        sortOptions2.addOption(dataSet.fieldByIndex(3), SortOrder::Ascending);
        dataSet.sort(sortOptions2);
        auto field3 = dataSet.fieldByIndex(3);

        lastVal = "";
        while (dataSet.next()) {
          REQUIRE(lastVal <= field3->getAsString());
          lastVal = field3->getAsString();
        }
      }

      AND_THEN("The data can be sorted by multiple keys") {
        DataSets::SortOptions sortOptions;
        sortOptions.addOption(dataSet.fieldByIndex(2), SortOrder::Ascending);
        sortOptions.addOption(dataSet.fieldByIndex(4), SortOrder::Descending);
        dataSet.sort(sortOptions);

        auto field2 = dataSet.fieldByIndex(2);
        auto field4 = dataSet.fieldByIndex(4);
        std::string str2Last, str4Last;
        while (dataSet.next()) {
          REQUIRE(str2Last <= field2->getAsString());
          if (str2Last == field2->getAsString()) {
            REQUIRE(str4Last >= field4->getAsString());
          }
          str2Last = field2->getAsString();
          str4Last = field4->getAsString();
        }
      }

      THEN("The data can be filtered") {
        DataSets::FilterOptions options;
        options.addOption(dataSet.fieldByIndex(2),
                          {"2", "5", "6"},
                          DataSets::FilterOption::Equals);
        auto filteredView = dataSet.filter(options);

        auto field2 = filteredView->fieldByIndex(2);

        while (filteredView->next()) {
          INFO("The value is " << field2->getAsString());
          REQUIRE((Utilities::compareString(field2->getAsString(), "2") == 0
              || Utilities::compareString(field2->getAsString(), "5") == 0
              || Utilities::compareString(field2->getAsString(), "6") == 0));
        }
      }

      AND_THEN("The data can be filtered by multiple keys") {
        DataSets::FilterOptions options;
        options.addOption(dataSet.fieldByIndex(2),
                          {"2", "5", "6"},
                          DataSets::FilterOption::Equals);
        options.addOption(dataSet.fieldByIndex(4),
                          {"1", "2", "3", "4", "5"},
                          DataSets::FilterOption::Equals);
        auto filteredView = dataSet.filter(options);

        auto field4 = filteredView->fieldByIndex(4);
        auto field2 = filteredView->fieldByIndex(2);

        while (filteredView->next()) {
          INFO("The value is " << field4->getAsString());
          REQUIRE((Utilities::compareString(field4->getAsString(), "1") == 0
              || Utilities::compareString(field4->getAsString(), "2") == 0
              || Utilities::compareString(field4->getAsString(), "3") == 0
              || Utilities::compareString(field4->getAsString(), "4") == 0
              || Utilities::compareString(field4->getAsString(), "5") == 0));
          INFO("The value is " << field2->getAsString());
          REQUIRE((Utilities::compareString(field2->getAsString(), "2") == 0
              || Utilities::compareString(field2->getAsString(), "5") == 0
              || Utilities::compareString(field2->getAsString(), "6") == 0));
        }
      }
    }

    WHEN("Reading the data from provider, integer values") {
      DataSets::MemoryDataSet dataSet("test_dataset");
      auto fieldTypes = {
          ValueType::String,
          ValueType::String,
          ValueType::Integer,
          ValueType::String,
          ValueType::Integer,
      };
      dataSet.open(provider, fieldTypes);

      THEN("The data inside data set correspond to data read") {
        auto fields = dataSet.getFields();
        REQUIRE(fields.size() == COL_COUNT);

        int rowCount = 0;
        while (dataSet.next()) {
          for (auto field : fields) {
            if (auto intField = dynamic_cast<DataSets::IntegerField *>(field);
                intField != nullptr) {
              CHECK(intField->getAsInteger()
                        == Utilities::stringToInt(data[rowCount][field->getIndex()]));
            } else {
              CHECK(field->getAsString() == data[rowCount][field->getIndex()]);
            }
          }
          rowCount++;
        }
        REQUIRE(rowCount == DATA_COUNT);

        rowCount--;
        while (dataSet.previous()) {
          auto wat = data[rowCount][0];
          for (auto field : fields) {
            if (auto intField = dynamic_cast<DataSets::IntegerField *>(field);
                intField != nullptr) {
              CHECK(intField->getAsInteger()
                        == Utilities::stringToInt(data[rowCount][field->getIndex()]));
            } else {
              CHECK(field->getAsString() == data[rowCount][field->getIndex()]);
            }
          }
          auto wat2 = data[rowCount][0];
          rowCount--;
        }
        REQUIRE(rowCount == -1);
      }

      THEN("The data can be sorted") {
        DataSets::SortOptions sortOptions;
        sortOptions.addOption(dataSet.fieldByIndex(2), SortOrder::Descending);
        dataSet.sort(sortOptions);
        auto field2 =
            dynamic_cast<DataSets::IntegerField *>(dataSet.fieldByIndex(2));
        int lastVal = std::numeric_limits<int>::max();
        while (dataSet.next()) {
          REQUIRE(lastVal >= field2->getAsInteger());
          lastVal = field2->getAsInteger();
        }

        DataSets::SortOptions sortOptions2;
        sortOptions2.addOption(dataSet.fieldByIndex(4), SortOrder::Ascending);
        dataSet.sort(sortOptions2);
        auto field4 =
            dynamic_cast<DataSets::IntegerField *>(dataSet.fieldByIndex(4));

        lastVal = std::numeric_limits<int>::min();
        while (dataSet.next()) {
          REQUIRE(lastVal <= field4->getAsInteger());
          lastVal = field4->getAsInteger();
        }
      }

      AND_THEN("The data can be sorted by multiple keys") {
        DataSets::SortOptions sortOptions;
        sortOptions.addOption(dataSet.fieldByIndex(2), SortOrder::Ascending);
        sortOptions.addOption(dataSet.fieldByIndex(4), SortOrder::Descending);
        dataSet.sort(sortOptions);

        auto field2 =
            dynamic_cast<DataSets::IntegerField *>(dataSet.fieldByIndex(2));
        auto field4 =
            dynamic_cast<DataSets::IntegerField *>(dataSet.fieldByIndex(4));
        int int2Last = std::numeric_limits<int>::min(),
            int4Last = std::numeric_limits<int>::max();
        while (dataSet.next()) {
          REQUIRE(int2Last <= field2->getAsInteger());
          if (int2Last == field2->getAsInteger()) {
            REQUIRE(int4Last >= field4->getAsInteger());
          }
          int2Last = field2->getAsInteger();
          int4Last = field4->getAsInteger();
        }
      }

      THEN("The data can be filtered") {
        DataSets::FilterOptions options;
        options.addOption(dataSet.fieldByIndex(2),
                          {"2", "5", "6"},
                          DataSets::FilterOption::Equals);
        auto filteredView = dataSet.filter(options);

        auto field2 =
            dynamic_cast<DataSets::IntegerField *>(filteredView->fieldByIndex(2));

        while (filteredView->next()) {
          INFO("The value is " << field2->getAsString());
          REQUIRE((Utilities::compareInt(field2->getAsInteger(), 2) == 0
              || Utilities::compareInt(field2->getAsInteger(), 5) == 0
              || Utilities::compareInt(field2->getAsInteger(), 6) == 0));
        }
      }

      AND_THEN("The data can be filtered by multiple keys") {
        DataSets::FilterOptions options;
        options.addOption(dataSet.fieldByIndex(2),
                          {"2", "5", "6"},
                          DataSets::FilterOption::Equals);
        options.addOption(dataSet.fieldByIndex(4),
                          {"1", "2", "3", "4", "5"},
                          DataSets::FilterOption::Equals);
        auto filteredView = dataSet.filter(options);

        auto field4 =
            dynamic_cast<DataSets::IntegerField *>(filteredView->fieldByIndex(4));
        auto field2 =
            dynamic_cast<DataSets::IntegerField *>(filteredView->fieldByIndex(2));

        while (filteredView->next()) {
          INFO("The value is " << field4->getAsInteger());
          REQUIRE((Utilities::compareInt(field4->getAsInteger(), 1) == 0
              || Utilities::compareInt(field4->getAsInteger(), 2) == 0
              || Utilities::compareInt(field4->getAsInteger(), 3) == 0
              || Utilities::compareInt(field4->getAsInteger(), 4) == 0
              || Utilities::compareInt(field4->getAsInteger(), 5) == 0));
          INFO("The value is " << field2->getAsInteger());
          REQUIRE((Utilities::compareInt(field2->getAsInteger(), 2) == 0
              || Utilities::compareInt(field2->getAsInteger(), 5) == 0
              || Utilities::compareInt(field2->getAsInteger(), 6) == 0));
        }
      }
    }

    WHEN("Reading the data from provider, double values") {
      DataSets::MemoryDataSet dataSet("test_dataset");
      auto fieldTypes = {
          ValueType::String,
          ValueType::String,
          ValueType::Double,
          ValueType::String,
          ValueType::Double,
      };
      dataSet.open(provider, fieldTypes);

      THEN("The data inside data set correspond to data read") {
        auto fields = dataSet.getFields();
        REQUIRE(fields.size() == COL_COUNT);

        int rowCount = 0;
        while (dataSet.next()) {
          for (auto field : fields) {
            if (auto doubleField = dynamic_cast<DataSets::DoubleField *>(field);
                doubleField != nullptr) {
              CHECK(doubleField->getAsDouble()
                        == Utilities::stringToDouble(data[rowCount][field->getIndex()]));
            } else {
              CHECK(field->getAsString() == data[rowCount][field->getIndex()]);
            }
          }
          rowCount++;
        }
        REQUIRE(rowCount == DATA_COUNT);

        rowCount--;
        while (dataSet.previous()) {
          auto wat = data[rowCount][0];
          for (auto field : fields) {
            if (auto doubleField = dynamic_cast<DataSets::DoubleField *>(field);
                doubleField != nullptr) {
              CHECK(doubleField->getAsDouble()
                        == Utilities::stringToDouble(data[rowCount][field->getIndex()]));
            } else {
              CHECK(field->getAsString() == data[rowCount][field->getIndex()]);
            }
          }
          auto wat2 = data[rowCount][0];
          rowCount--;
        }
        REQUIRE(rowCount == -1);
      }

      THEN("The data can be sorted") {
        DataSets::SortOptions sortOptions;
        sortOptions.addOption(dataSet.fieldByIndex(2), SortOrder::Descending);
        dataSet.sort(sortOptions);
        auto field2 =
            dynamic_cast<DataSets::DoubleField *>(dataSet.fieldByIndex(2));
        double lastVal = std::numeric_limits<double>::max();
        while (dataSet.next()) {
          REQUIRE(lastVal >= field2->getAsDouble());
          lastVal = field2->getAsDouble();
        }

        DataSets::SortOptions sortOptions2;
        sortOptions2.addOption(dataSet.fieldByIndex(4), SortOrder::Ascending);
        dataSet.sort(sortOptions2);
        auto field4 =
            dynamic_cast<DataSets::DoubleField *>(dataSet.fieldByIndex(4));

        lastVal = -1;
        while (dataSet.next()) {
          REQUIRE(lastVal <= field4->getAsDouble());
          lastVal = field4->getAsDouble();
        }
      }

      AND_THEN("The data can be sorted by multiple keys") {
        DataSets::SortOptions sortOptions;
        sortOptions.addOption(dataSet.fieldByIndex(2), SortOrder::Ascending);
        sortOptions.addOption(dataSet.fieldByIndex(4), SortOrder::Descending);
        dataSet.sort(sortOptions);

        auto field2 =
            dynamic_cast<DataSets::DoubleField *>(dataSet.fieldByIndex(2));
        auto field4 =
            dynamic_cast<DataSets::DoubleField *>(dataSet.fieldByIndex(4));
        double double2Last = -1;
        double double4Last = std::numeric_limits<double>::max();
        while (dataSet.next()) {
          REQUIRE(double2Last <= field2->getAsDouble());
          if (double2Last == field2->getAsDouble()) {
            REQUIRE(double4Last >= field4->getAsDouble());
          }
          double2Last = field2->getAsDouble();
          double4Last = field4->getAsDouble();
        }
      }

      THEN("The data can be filtered") {
        DataSets::FilterOptions options;
        options.addOption(dataSet.fieldByIndex(2),
                          {"2", "5", "6"},
                          DataSets::FilterOption::Equals);
        auto filteredView = dataSet.filter(options);

        auto field2 =
            dynamic_cast<DataSets::DoubleField *>(filteredView->fieldByIndex(2));

        while (filteredView->next()) {
          INFO("The value is " << field2->getAsDouble());
          REQUIRE((Utilities::compareDouble(field2->getAsDouble(), 2) == 0
              || Utilities::compareDouble(field2->getAsDouble(), 5) == 0
              || Utilities::compareDouble(field2->getAsDouble(), 6) == 0));
        }
      }

      AND_THEN("The data can be filtered by multiple keys") {
        DataSets::FilterOptions options;
        options.addOption(dataSet.fieldByIndex(2),
                          {"2", "5", "6"},
                          DataSets::FilterOption::Equals);
        options.addOption(dataSet.fieldByIndex(4),
                          {"1", "2", "3", "4", "5"},
                          DataSets::FilterOption::Equals);
        auto filteredView = dataSet.filter(options);

        auto field4 =
            dynamic_cast<DataSets::DoubleField *>(filteredView->fieldByIndex(4));
        auto field2 =
            dynamic_cast<DataSets::DoubleField *>(filteredView->fieldByIndex(2));

        while (filteredView->next()) {
          INFO("The value is " << field4->getAsDouble());
          REQUIRE((Utilities::compareDouble(field4->getAsDouble(), 1) == 0
              || Utilities::compareDouble(field4->getAsDouble(), 2) == 0
              || Utilities::compareDouble(field4->getAsDouble(), 3) == 0
              || Utilities::compareDouble(field4->getAsDouble(), 4) == 0
              || Utilities::compareDouble(field4->getAsDouble(), 5) == 0));
          INFO("The value is " << field2->getAsDouble());
          REQUIRE((Utilities::compareDouble(field2->getAsDouble(), 2) == 0
              || Utilities::compareDouble(field2->getAsDouble(), 5) == 0
              || Utilities::compareDouble(field2->getAsDouble(), 6) == 0));
        }
      }
    }

    WHEN("Reading the data from provider, currency values") {
      DataSets::MemoryDataSet dataSet("test_dataset");
      auto fieldTypes = {
          ValueType::String,
          ValueType::String,
          ValueType::Currency,
          ValueType::String,
          ValueType::Currency,
      };
      dataSet.open(provider, fieldTypes);

      THEN("The data inside data set correspond to data read") {
        auto fields = dataSet.getFields();
        REQUIRE(fields.size() == COL_COUNT);

        int rowCount = 0;
        while (dataSet.next()) {
          for (auto field : fields) {
            if (auto currencyField =
                  dynamic_cast<DataSets::CurrencyField *>(field); currencyField
                != nullptr) {
              CHECK(currencyField->getAsCurrency()
                        == dec::fromString<Currency>(data[rowCount][field->getIndex()]));
            } else {
              CHECK(field->getAsString() == data[rowCount][field->getIndex()]);
            }
          }
          rowCount++;
        }
        REQUIRE(rowCount == DATA_COUNT);

        rowCount--;
        while (dataSet.previous()) {
          auto wat = data[rowCount][0];
          for (auto field : fields) {
            if (auto currencyField =
                  dynamic_cast<DataSets::CurrencyField *>(field); currencyField
                != nullptr) {
              CHECK(currencyField->getAsCurrency()
                        == dec::fromString<Currency>(data[rowCount][field->getIndex()]));
            } else {
              CHECK(field->getAsString() == data[rowCount][field->getIndex()]);
            }
          }
          auto wat2 = data[rowCount][0];
          rowCount--;
        }
        REQUIRE(rowCount == -1);
      }

      THEN("The data can be sorted") {
        DataSets::SortOptions sortOptions;
        sortOptions.addOption(dataSet.fieldByIndex(2), SortOrder::Descending);
        dataSet.sort(sortOptions);
        auto field2 =
            dynamic_cast<DataSets::CurrencyField *>(dataSet.fieldByIndex(2));
        Currency lastVal("10000000");
        while (dataSet.next()) {
          REQUIRE(lastVal >= field2->getAsCurrency());
          lastVal = field2->getAsCurrency();
        }

        DataSets::SortOptions sortOptions2;
        sortOptions2.addOption(dataSet.fieldByIndex(4), SortOrder::Ascending);
        dataSet.sort(sortOptions2);
        auto field4 =
            dynamic_cast<DataSets::CurrencyField *>(dataSet.fieldByIndex(4));

        lastVal = dec::fromString<Currency>("0");
        while (dataSet.next()) {
          REQUIRE(lastVal <= field4->getAsCurrency());
          lastVal = field4->getAsCurrency();
        }
      }

      AND_THEN("The data can be sorted by multiple keys") {
        DataSets::SortOptions sortOptions;
        sortOptions.addOption(dataSet.fieldByIndex(2), SortOrder::Ascending);
        sortOptions.addOption(dataSet.fieldByIndex(4), SortOrder::Descending);
        dataSet.sort(sortOptions);

        auto field2 =
            dynamic_cast<DataSets::CurrencyField *>(dataSet.fieldByIndex(2));
        auto field4 =
            dynamic_cast<DataSets::CurrencyField *>(dataSet.fieldByIndex(4));
        Currency cur2Last = dec::fromString<Currency>("0");
        Currency cur4Last = dec::fromString<Currency>("100000000");
        while (dataSet.next()) {
          REQUIRE(cur2Last <= field2->getAsCurrency());
          if (cur2Last == field2->getAsCurrency()) {
            REQUIRE(cur4Last >= field4->getAsCurrency());
          }
          cur2Last = field2->getAsCurrency();
          cur4Last = field4->getAsCurrency();
        }
      }

      THEN("The data can be filtered") {
        DataSets::FilterOptions options;
        options.addOption(dataSet.fieldByIndex(2),
                          {"2", "5", "6"},
                          DataSets::FilterOption::Equals);
        auto filteredView = dataSet.filter(options);

        auto field2 =
            dynamic_cast<DataSets::CurrencyField *>(filteredView->fieldByIndex(2));

        while (filteredView->next()) {
          INFO("The value is " << field2->getAsCurrency());
          REQUIRE((Utilities::compareCurrency(field2->getAsCurrency(),
                                              Currency(2)) == 0
              || Utilities::compareCurrency(field2->getAsCurrency(),
                                            Currency(5)) == 0
              || Utilities::compareCurrency(field2->getAsCurrency(),
                                            Currency(6)) == 0));
        }
      }

      AND_THEN("The data can be filtered by multiple keys") {
        DataSets::FilterOptions options;
        options.addOption(dataSet.fieldByIndex(2),
                          {"2", "5", "6"},
                          DataSets::FilterOption::Equals);
        options.addOption(dataSet.fieldByIndex(4),
                          {"1", "2", "3", "4", "5"},
                          DataSets::FilterOption::Equals);
        auto filteredView = dataSet.filter(options);

        auto field4 =
            dynamic_cast<DataSets::CurrencyField *>(filteredView->fieldByIndex(4));
        auto field2 =
            dynamic_cast<DataSets::CurrencyField *>(filteredView->fieldByIndex(2));

        while (filteredView->next()) {
          INFO("The value is " << field4->getAsCurrency());
          REQUIRE((Utilities::compareCurrency(field4->getAsCurrency(),
                                              Currency(1)) == 0
              || Utilities::compareCurrency(field4->getAsCurrency(),
                                            Currency(2)) == 0
              || Utilities::compareCurrency(field4->getAsCurrency(),
                                            Currency(3)) == 0
              || Utilities::compareCurrency(field4->getAsCurrency(),
                                            Currency(4)) == 0
              || Utilities::compareCurrency(field4->getAsCurrency(),
                                            Currency(5)) == 0));
          INFO("The value is " << field2->getAsCurrency());
          REQUIRE((Utilities::compareCurrency(field2->getAsCurrency(),
                                              Currency(2)) == 0
              || Utilities::compareCurrency(field2->getAsCurrency(),
                                            Currency(5)) == 0
              || Utilities::compareCurrency(field2->getAsCurrency(),
                                            Currency(6)) == 0));
        }
      }
    }
  }
}