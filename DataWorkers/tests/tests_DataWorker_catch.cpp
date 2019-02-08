//
// Created by Petr Flajsingr on 2019-02-08.
//

#include <catch.hpp>
#include <MemoryDataWorker.h>
#include <ArrayDataProvider.h>
#include <ArrayWriter.h>

SCENARIO("Basic input", "[DataWorkers]") {
  GIVEN("Simple input") {
    const std::vector<std::vector<std::string>> test{
        {"COLUMN11", "10", "1", "COLUMN14", "COLUMN15"},
        {"COLUMN21", "10", "1", "COLUMN24", "COLUMN25"},
        {"COLUMN31", "10", "1", "COLUMN34", "COLUMN35"},
        {"COLUMN41", "10", "1", "COLUMN44", "COLUMN45"},
        {"COLUMN51", "20", "1", "COLUMN54", "COLUMN55"},
        {"COLUMN22", "20", "1", "COLUMN54", "COLUMN55"},
        {"COLUMN22", "20", "1", "COLUMN54", "COLUMN55"},
        {"COLUMN22", "20", "1", "COLUMN54", "COLUMN55"}
    };

    const std::vector<std::vector<std::string>> basicTest{
        {"COLUMN11", "10", "1", "COLUMN14", "COLUMN15"},
        {"COLUMN21", "10", "1", "COLUMN24", "COLUMN25"},
        {"COLUMN31", "10", "1", "COLUMN34", "COLUMN35"},
        {"COLUMN41", "10", "1", "COLUMN44", "COLUMN45"},
        {"COLUMN51", "20", "1", "COLUMN54", "COLUMN55"},
    };

    const std::vector<std::vector<std::string>> joinTest{
        {"10", "deset"},
        {"20", "dvacet"},
    };

    const std::vector<std::vector<std::string>> mixedTest{
        {"0", "1", "2(Sum)", "3", "4", "1"},
        {"COLUMN11", "10", "1", "COLUMN14", "COLUMN15"},
        {"COLUMN21", "10", "1", "COLUMN24", "COLUMN25"},
        {"COLUMN22", "20", "3", "COLUMN54", "COLUMN55"},
        {"COLUMN31", "10", "1", "COLUMN34", "COLUMN35"},
        {"COLUMN41", "10", "1", "COLUMN44", "COLUMN45"},
        {"COLUMN51", "20", "1", "COLUMN54", "COLUMN55"}
    };

    const std::vector<std::vector<std::string>> joinAnswers{
        {"0", "1", "2(Sum)", "3", "4", "1"},
        {"COLUMN11", "10", "1", "COLUMN14", "COLUMN15", "deset"},
        {"COLUMN21", "10", "1", "COLUMN24", "COLUMN25", "deset"},
        {"COLUMN22", "20", "3", "COLUMN54", "COLUMN55", "dvacet"},
        {"COLUMN31", "10", "1", "COLUMN34", "COLUMN35", "deset"},
        {"COLUMN41", "10", "1", "COLUMN44", "COLUMN45", "deset"},
        {"COLUMN51", "20", "1", "COLUMN54", "COLUMN55", "dvacet"}
    };

    const std::vector<std::vector<std::string>> advTest{
        {"prvni", "2839", "10.01", "444.8", "27", "8"},
        {"special", "33", "90.00", "666.8", "27", "2"},
        {"special", "44", "90.00", "666.8", "27", "2"},
        {"druhy", "2422", "200.02", "222.8", "27", "3"},
        {"prvni", "2839", "3000.03", "333.8", "27", "9"},
        {"treti", "2331", "40000.04", "444.8", "27", "11"},
        {"ctvrty", "22222", "90.00", "666.8", "27", "2"},
        {"druhy", "2839", "500000.05", "555.8", "27", "5"},
        {"prvni", "2222", "600000.06", "111.8", "27", "6"},
        {"ctvrty", "2839", "70000.07", "222.8", "27", "1"},
        {"prvni", "2222", "8000.08", "333.8", "27", "7"},
        {"treti", "88", "900.09", "555.8", "27", "10"},
        {"special", "11", "1.00", "666.8", "27", "2"},
        {"special", "22", "1.00", "666.8", "27", "2"},
        {"special", "22", "1.00", "666.8", "27", "2"},
        {"special", "11", "1.00", "666.8", "27", "2"},
        {"special", "11", "1.00", "666.8", "27", "2"},
    };

    const std::vector<std::string> advUnique0{
        "ctvrty",
        "druhy",
        "prvni",
        "special",
        "treti",
    };

    const std::vector<std::string> advUnique1{
        "11",
        "22",
        "33",
        "44",
        "88",
        "2222",
        "2331",
        "2422",
        "2839",
        "22222",
    };

    const std::vector<std::vector<std::string>> advAnswers{
        {"0", "1", "2(Sum)", "3(Sum)", "4(Sum)", "5(Sum)"},
        {"ctvrty", "2839", "70000.07", "222.80", "27", "1.00"},
        {"ctvrty", "22222", "90.00", "666.80", "27", "2.00"},
        {"druhy", "2422", "200.02", "222.80", "27", "3.00"},
        {"druhy", "2839", "500000.05", "555.80", "27", "5.00"},
        {"prvni", "2222", "608000.14", "445.60", "54", "13.00"},
        {"prvni", "2839", "3010.04", "778.60", "54", "17.00"},
        {"special", "11", "3.00", "2000.40", "81", "6.00"},
        {"special", "22", "2.00", "1333.60", "54", "4.00"},
        {"special", "33", "90.00", "666.80", "27", "2.00"},
        {"special", "44", "90.00", "666.80", "27", "2.00"},
        {"treti", "88", "900.09", "555.80", "27", "10.00"},
        {"treti", "2331", "40000.04", "444.80", "27", "11.00"},
    };

    DataProviders::ArrayDataProvider dataProvider(test);
    DataProviders::ArrayDataProvider advDataProvider(advTest);

    THEN("Unique choices") {

      DataWorkers::MemoryDataWorker worker(dataProvider,
                                           {ValueType::String,
                                            ValueType::Integer,
                                            ValueType::String,
                                            ValueType::String,
                                            ValueType::String});

      std::vector<std::string> choices{"COLUMN11", "COLUMN13"};
      worker.setColumnChoices(choices);

      int i = 0;
      for (auto &str : worker.getMultiChoiceNames()) {
        CHECK(str == choices[i]);
        ++i;
      }
    }

    THEN("Basic distinct") {
      DataProviders::ArrayDataProvider provider(basicTest);
      DataWorkers::MemoryDataWorker
          worker(provider,
                 {ValueType::String,
                  ValueType::Integer,
                  ValueType::String,
                  ValueType::String,
                  ValueType::String});

      std::string sql = "SELECT main.0 FROM main";

      DataWriters::ArrayWriter writer;

      worker.writeResult(writer, sql);

      CHECK((writer.result)[0][0] == "0");
      for (int i = 1; i < writer.result.size(); ++i) {
        CHECK((writer.result)[i][0] == basicTest[i - 1][0]);
      }
    }

    THEN("Advanced distinct") {
      DataWorkers::MemoryDataWorker worker(advDataProvider,
                                           {ValueType::String,
                                            ValueType::Integer,
                                            ValueType::Currency,
                                            ValueType::Currency,
                                            ValueType::Integer,
                                            ValueType::Currency});

      std::string sql =
          "SELECT main.0, main.1, SUM(main.2), SUM(main.3), SUM(main.4), SUM(main.5) FROM main";

      DataWriters::ArrayWriter writer;

      worker.writeResult(writer, sql);

      for (int i = 0; i < advTest[0].size(); ++i) {
        CHECK((writer.result)[0][i] == advAnswers[0][i]);
      }
    }

    THEN("Basic sum") {
      DataProviders::ArrayDataProvider provider(basicTest);

      DataWorkers::MemoryDataWorker worker(provider,
                                           {ValueType::String,
                                            ValueType::Integer,
                                            ValueType::String,
                                            ValueType::String,
                                            ValueType::String});

      std::string sql = "SELECT main.0, SUM(main.1) FROM main";

      DataWriters::ArrayWriter writer;

      worker.writeResult(writer, sql);

      CHECK((writer.result)[0][1] == "1(Sum)");
      for (int i = 1; i < writer.result.size(); ++i) {
        CHECK((writer.result)[i][1] == basicTest[i - 1][1]);
      }
    }

    THEN("Advanced sum") {
      DataWorkers::MemoryDataWorker worker(advDataProvider,
                                           {ValueType::String,
                                            ValueType::Integer,
                                            ValueType::Currency,
                                            ValueType::Currency,
                                            ValueType::Integer,
                                            ValueType::Currency});

      std::string sql =
          "SELECT main.0, main.1, SUM(main.2), SUM(main.3), SUM(main.4), SUM(main.5) FROM main";

      DataWriters::ArrayWriter writer;

      worker.writeResult(writer, sql);

      for (int i = 0; i < (writer.result).size(); ++i) {
        for (int j = 0; j < (writer.result)[i].size(); ++j) {
          CHECK((writer.result)[i][j] == advAnswers[i][j]);
        }
      }
    }

    THEN("Basic average") {
      DataProviders::ArrayDataProvider provider(basicTest);

      DataWorkers::MemoryDataWorker worker(provider,
                                           {ValueType::String,
                                            ValueType::Integer,
                                            ValueType::Integer,
                                            ValueType::String,
                                            ValueType::String});

      std::string sql = "SELECT main.0, AVG(main.2) FROM main";

      DataWriters::ArrayWriter writer;

      worker.writeResult(writer, sql);

      CHECK((writer.result)[0][1] == "2(Avg)");
      for (int i = 1; i < writer.result.size(); ++i) {
        CHECK((writer.result)[i][1] == basicTest[i - 1][2]);
      }
    }

    THEN("Selection") {
      DataWorkers::MemoryDataWorker worker(advDataProvider,
                                           {ValueType::String,
                                            ValueType::Integer,
                                            ValueType::Currency,
                                            ValueType::Currency,
                                            ValueType::Integer,
                                            ValueType::Currency});

      std::string sql =
          "SELECT main.0, AVG(main.2) FROM main WHERE main.0 = prvni | druhy";

      DataWriters::ArrayWriter writer;

      worker.writeResult(writer, sql);

      CHECK((writer.result)[0][0] == "0");
      for (int i = 1; i < writer.result.size(); ++i) {
        CHECK(((writer.result)[i][0] == "prvni" ||
            (writer.result)[i][0] == "druhy"));
      }
    }

    THEN("Mix") {
      DataWorkers::MemoryDataWorker worker(dataProvider,
                                           {ValueType::String,
                                            ValueType::Integer,
                                            ValueType::Integer,
                                            ValueType::String,
                                            ValueType::String});

      std::string
          sql = "SELECT main.0, main.1, SUM(main.2), main.3, main.4 FROM main";

      DataProviders::ArrayDataProvider joinProvider(joinTest);

      DataWriters::ArrayWriter writer;

      worker.writeResult(writer, sql);

      for (int i = 0; i < (writer.result).size(); ++i) {
        for (int j = 0; j < (writer.result)[i].size(); ++j) {
          CHECK((writer.result)[i][j] == mixedTest[i][j]);
        }
      }
    }

    THEN("Join") {
      DataWorkers::MemoryDataWorker worker(dataProvider,
                                           {ValueType::String,
                                            ValueType::Integer,
                                            ValueType::Integer,
                                            ValueType::String,
                                            ValueType::String});

      std::string sql =
          "SELECT main.0, main.1, SUM(main.2), main.3, main.4, joinTest.1 FROM main JOIN joinTest ON main.1 = joinTest.0";

      DataProviders::ArrayDataProvider joinProvider(joinTest);
      DataSets::MemoryDataSet joinDataSet("joinTest");
      joinDataSet.open(joinProvider, {ValueType::Integer, ValueType::String});

      worker.addDataSet(&joinDataSet);

      DataWriters::ArrayWriter writer;

      worker.writeResult(writer, sql);

      for (int i = 0; i < (writer.result).size(); ++i) {
        for (int j = 0; j < (writer.result)[i].size(); ++j) {
          CHECK((writer.result)[i][j] == joinAnswers[i][j]);
        }
      }
    }

    THEN("Unique values") {
      DataWorkers::MemoryDataWorker worker(advDataProvider,
                                           {ValueType::String,
                                            ValueType::Integer,
                                            ValueType::Currency,
                                            ValueType::Currency,
                                            ValueType::Integer,
                                            ValueType::Currency});

      std::vector<std::string> cols = {"0", "1"};
      worker.setColumnChoices(cols);

      auto unique0 = worker.getChoices("0");
      auto unique1 = worker.getChoices("1");

      CHECK(unique0.size() == advUnique0.size());
      for (int i = 0; i < advUnique0.size(); i++) {
        CHECK(advUnique0[i] == unique0[i]);
      }

      CHECK(unique1.size() == advUnique1.size());
      for (int i = 0; i < advUnique1.size(); i++) {
        CHECK(advUnique1[i] == unique1[i]);
      }
    }
  }
}