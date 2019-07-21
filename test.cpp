//
// Created by Petr Flajsingr on 2019-02-10.
//

#include <MemoryDataSet.h>
#include <CLIController.h>
#include "Templates/Headers/Observable.h"
#include <Bindable.h>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>
#include <ArrayDataProvider.h>
#include <StdoutWriter.h>
#include <CsvReader.h>
#include <XlntReader.h>
#include <XlsxIOReader.h>
#include "Combiner.h"

void terminate_handler();

int main(int argc, char **argv) {
  //CLIController cl;
  //cl.runApp();

  DataBase::MemoryDataBase db("jo");

  auto providerRes = DataProviders::XlsxIOReader("/Users/petr/Desktop/muni_last/NNO_RES_2019-03-30.xlsx");
  auto dsRes = std::make_shared<DataSets::MemoryDataSet>("res");
  dsRes->open(providerRes, {ValueType::Integer,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String});
  db.addTable(dsRes);

  auto providerSbirky = DataProviders::CsvReader("/Users/petr/Desktop/muni_last/Verejne_sbirky.csv", ";");
  auto dsSbirky = std::make_shared<DataSets::MemoryDataSet>("sbirky");
  dsSbirky->open(providerSbirky, {ValueType::String,
                                  ValueType::String,
                                  ValueType::Integer,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String});
  db.addTable(dsSbirky);

  auto result = db.execSimpleQuery(R"(select res.*, sbirky.* from res join sbirky on res.ICO=sbirky.IČO:)",
                                   false,
                                   "test");

  std::cout << "done";
}

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
