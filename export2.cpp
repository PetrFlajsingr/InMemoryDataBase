//
// Created by petr on 10/7/19.
//

#include "Combiner.h"
#include <CsvStreamReader.h>
#include <CsvWriter.h>
#include <Logger.h>
#include <MemoryDataBase.h>
#include <MemoryDataSet.h>
#include <XlntReader.h>
#include <XlntWriter.h>
#include <XlsxIOReader.h>
#include <XlsReader.h>
#include <memory>

using namespace std::string_literals;

auto logger = Logger<true>::GetInstance();

struct FileSettings {
  enum Type { csv, xlsx, xls };
  Type type;
  std::string pathToFile;
  char delimiter;
  std::string sheet = "";

  static FileSettings Xlsx(const std::string &path, const std::string &sheet = "") {
    FileSettings result;
    result.type = xlsx;
    result.pathToFile = path;
    result.sheet = sheet;
    return result;
  }

  static FileSettings Xls(const std::string &path, const std::string &sheet = "") {
    FileSettings result;
    result.type = xls;
    result.pathToFile = path;
    result.sheet = sheet;
    return result;
  }

  static FileSettings Csv(const std::string &path, char delimiter = ',') {
    FileSettings result;
    result.type = csv;
    result.pathToFile = path;
    result.delimiter = delimiter;
    return result;
  }
};

std::shared_ptr<DataSets::MemoryDataSet> createDataSetFromFile(const std::string &name,
                                                               const FileSettings &fileSettings,
                                                               const std::vector<ValueType> &valueTypes) {
  std::unique_ptr<DataProviders::BaseDataProvider> provider;
  switch (fileSettings.type) {
  case FileSettings::csv:
    provider = std::make_unique<DataProviders::CsvStreamReader>(fileSettings.pathToFile, fileSettings.delimiter);
    break;
  case FileSettings::xlsx:
    provider = std::make_unique<DataProviders::XlsxIOReader>(fileSettings.pathToFile);
    if (!fileSettings.sheet.empty()) {
      dynamic_cast<DataProviders::XlsxIOReader *>(provider.get())->openSheet(fileSettings.sheet);
    }
    break;
  case FileSettings::xls:
    provider = std::make_unique<DataProviders::XlntReader>(fileSettings.pathToFile);
    break;
  }
  auto result = std::make_shared<DataSets::MemoryDataSet>(name);
  result->open(*provider, valueTypes);
  logger.log<LogLevel::Status>("Loaded: "s + result->getName());
  result->resetEnd();
  logger.log<LogLevel::Debug>("Count:", result->getCurrentRecord());
  result->resetBegin();
  return result;
}

const auto folder = "/home/petr/Desktop/export2/"s;

int main() {
  DataBase::MemoryDataBase db("db");

  auto subjekty = createDataSetFromFile("subjekty", FileSettings::Xlsx(folder + "NNO.xlsx"),
                                        {ValueType::Integer, ValueType::Integer, ValueType::String, ValueType::String,
                                         ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                         ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                         ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                         ValueType::String});
  db.addTable(subjekty);
  db.execSimpleQuery("select subjekty.* from subjekty;", true, "sub");

  auto VZds = createDataSetFromFile(
      "VZ", FileSettings::Xlsx(folder + "vz.xlsx", "VZ"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String});

  auto castiVZds = createDataSetFromFile(
      "castiVZ", FileSettings::Xlsx(folder + "vz.xlsx", "Části VZ"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String});
  auto dodavatelVZ = createDataSetFromFile(
      "dodVZ", FileSettings::Xlsx(folder + "vz.xlsx", "Dodavatelé"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String});

  auto NNO = createDataSetFromFile("subjekty", FileSettings::Xlsx(folder + "NNO.xlsx"),
                                   {ValueType::Integer, ValueType::Integer, ValueType::String, ValueType::String,
                                    ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                    ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                    ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                    ValueType::String});
  db.addTable(NNO);

  db.addTable(VZds);
  db.addTable(castiVZds);
  db.addTable(dodavatelVZ);

  const std::string query = "select VZ.*, castiVZ.*, dodVZ.* from VZ "
                            "join castiVZ on VZ.ID_Zakazky = castiVZ.ID_Zakazky "
                            "join dodVZ on VZ.ID_Zakazky = dodVZ.ID_Zakazky "
                            "join subjekty on dodVZ.DodavatelICO = subjekty.ICOnum;";

  db.execSimpleQuery(query, true, "vz");
  logger.log<LogLevel::Info>("Query vz done");

  auto cedr = createDataSetFromFile(
      "cedr", FileSettings::Xlsx(folder + "cedr.xlsx"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::Integer,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String});
  db.addTable(cedr);
  db.execSimpleQuery("select cedr.* from cedr;", true, "cedr2");
  logger.log<LogLevel::Info>("Query cedr done");

  auto dotaceDS = createDataSetFromFile(
      "dotace", FileSettings::Xls(folder + "dotaceeu.xls"),
      {ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String,  ValueType::String, ValueType::String});
  db.addTable(dotaceDS);
  db.execSimpleQuery("select dotace.* from dotace join subjekty on dotace.IČ = subjekty.ICOnum;", true, "dotace2");
  logger.log<LogLevel::Info>("Query dotace2done");

  db.execSimpleQuery("select vz.* from vz join subjekty on vz.DodavatelICO = subjekty.ICOnum;", true, "vz2");
  logger.log<LogLevel::Info>("Query vz2 done");

  Combiner combiner;

  combiner.addDataSource(db.viewByName("sub"));
  combiner.addDataSource(db.viewByName("vz2"));
  combiner.addDataSource(db.viewByName("cedr2"));
  combiner.addDataSource(db.viewByName("dotace2"));

  auto result = combiner.combineOn({{"sub", "ICOnum"}, {"vz2", "DodavatelICO"}, {"cedr2", "ico"}, {"dotace2", "IČ"}});

  DataWriters::XlntWriter writer(folder + "export2.xlsx");

  writer.writeHeader(result->dataSet->getFieldNames());
  auto fields = result->dataSet->getFields();
  result->dataSet->resetBegin();

  std::vector<std::string> record;
  record.reserve(fields.size());
  while (result->dataSet->next()) {
    std::transform(fields.begin(), fields.end(), std::back_inserter(record),
                   [](const DataSets::BaseField *field) { return field->getAsString(); });
    writer.writeRecord(record);
    record.clear();
  }
}