//
// Created by Petr Flajsingr on 2019-02-10.
//

#include <MemoryDataSet.h>
#include <LexicalAnalyser.h>
#include <SyntaxAnalyser.h>
#include <SemanticAnalyser.h>
#include <MemoryDataBase.h>
#include <JoinMaker.h>
#include <CsvReader.h>

int main() {
  const std::string file1 = "/Users/petr/Desktop/join_test_1.csv";
  const std::string file2 = "/Users/petr/Desktop/join_test_2.csv";
  const std::string outFile = "/Users/petr/Desktop/join_test_out.csv";
  auto prov1 = DataProviders::CsvReader(file1);
  auto prov2 = DataProviders::CsvReader(file2);
  auto ds1 = std::make_shared<DataSets::MemoryDataSet>("test1");
  ds1->open(prov1, {ValueType::String, ValueType::String, ValueType::Integer});
  auto ds2 = std::make_shared<DataSets::MemoryDataSet>("test2");
  ds2->open(prov2, {ValueType::String, ValueType::String, ValueType::Integer});

  auto table1 = std::make_shared<DataBase::Table>(ds1);
  auto table2 = std::make_shared<DataBase::Table>(ds2);
  DataBase::JoinMaker joinMaker(table1, "A1", table2, "A");
  auto joinResult = joinMaker.join(DataBase::JoinType::leftJoin);

  auto view = std::make_shared<DataBase::View>(joinResult);

  DataBase::JoinMaker joinMaker1(view, "A1", table2, "A");
  joinResult = joinMaker1.join(DataBase::JoinType::innerJoin);

  auto writer = DataWriters::CsvWriter(outFile);

  writer.writeHeader(joinResult->getFieldNames());

  auto fields = joinResult->getFields();
  while (joinResult->next()) {
    std::vector<std::string> record;
    std::transform(fields.begin(),
                   fields.end(),
                   std::back_inserter(record),
                   [](const DataSets::BaseField *field) {
                     return std::string(field->getAsString());
                   });
    writer.writeRecord(record);
  }

  return 0;

}