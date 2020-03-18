//
// Created by petr on 3/18/20.
//
#include "LoadingUtils.h"
#include "MemoryDataBase.h"
#include "StdoutWriter.h"
int main() {
  DataBase::MemoryDataBase db("db");
  db.addTable(createDataSetFromFile("t1", FileSettings::Csv("/home/petr/Desktop/test_join/t1.csv"),
                                    {ValueType::String, ValueType::String}));
  db.addTable(createDataSetFromFile("t2", FileSettings::Csv("/home/petr/Desktop/test_join/t2.csv"),
                                    {ValueType::String, ValueType::String}));
  db.addTable(createDataSetFromFile("t3", FileSettings::Csv("/home/petr/Desktop/test_join/t3.csv"),
                                    {ValueType::String, ValueType::String}));

  const auto query = R"(SELECT t1.*, t2.*, t3.* FROM t1
LEFT JOIN t2 ON t1.A = t2.A2
LEFT JOIN t3 ON t1.A = t3.A3;)";
  auto result = db.execSimpleQuery(query, false, "t")->dataSet;
  DataWriters::StdoutWriter writer;
  writer.writeDataSet(*result);


}