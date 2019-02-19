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

void terminate_handler() {
  try {
    auto unknown = std::current_exception();
    if (unknown) {
      std::rethrow_exception(unknown);
    } else {
      std::cerr << "normal termination" << std::endl;
    }
  } catch (const std::exception &e) {
    std::cerr << "Exc type: " << typeid(e).name() << "\nExc.what: " << e.what()
              << std::endl;
  } catch (...) {
    std::cerr << "Unknown exception of type" << std::endl;
  }
  abort();
}

int main() {
  //std::set_terminate(terminate_handler);

  const std::string file1 = "/Users/petr/Desktop/join_test_1.csv";
  const std::string file2 = "/Users/petr/Desktop/join_test_2.csv";
  const std::string outFile = "/Users/petr/Desktop/join_test_out.csv";
  auto prov1 = DataProviders::CsvReader(file1);
  auto prov2 = DataProviders::CsvReader(file2);
  auto ds1 = std::make_shared<DataSets::MemoryDataSet>("test1");
  ds1->open(prov1, {ValueType::String, ValueType::String, ValueType::Integer});
  auto ds2 = std::make_shared<DataSets::MemoryDataSet>("test2");
  ds2->open(prov2, {ValueType::String, ValueType::String, ValueType::Integer});

  /*auto table1 = std::make_shared<DataBase::Table>(ds1);
  auto table2 = std::make_shared<DataBase::Table>(ds2);
  DataBase::JoinMaker joinMaker(table1, "A1", table2, "A");
  auto joinResult = joinMaker.join(DataBase::JoinType::leftJoin);

  DataBase::JoinMaker joinMaker1(joinResult, "A1", table2, "A");
  joinResult = joinMaker1.join(DataBase::JoinType::innerJoin);*/

  DataBase::MemoryDataBase db("testDB");
  db.addTable(ds1);
  db.addTable(ds2);

  const std::string query = "select test1.A1, test1.B1, test1.C1, "
                            "test2.A, test2.B, test2.C "
                            "from test1 join test2 on test1.A1 = test2.A "
                            "where test1.A1 = \"A\" | \"H\";";

  auto view = db.execSimpleQuery(query, false, "tmpView");

  auto writer = DataWriters::CsvWriter(outFile);

  writer.writeHeader(view->dataSet->getFieldNames());

  auto fields = view->dataSet->getFields();
  while (view->dataSet->next()) {
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