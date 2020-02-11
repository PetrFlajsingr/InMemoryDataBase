//
// Created by petr on 2/11/20.
//

#include "Misc/Headers/Logger.h"
#include <CsvReader.h>
#include <CsvStreamReader.h>
#include <CsvWriter.h>
#include <MemoryDataBase.h>
#include <XlsxIOReader.h>
#include <XlsxReader.h>

using namespace std::string_literals;

auto &logger = Logger<true>::GetInstance();

struct FileSettings {
  enum Type { csv, xlsx, xls, csvOld };
  Type type;
  std::string pathToFile;
  char delimiter;
  std::string sheet = "";
  bool useCharset = false;
  CharSet charset;

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

  static FileSettings Csv(const std::string &path, char delimiter = ',', bool useCharset = false,
                          CharSet charset = CharSet::CP1250) {
    FileSettings result;
    result.type = csv;
    result.pathToFile = path;
    result.delimiter = delimiter;
    result.useCharset = useCharset;
    result.charset = charset;
    return result;
  }

  static FileSettings CsvOld(const std::string &path, char delimiter = ',', bool useCharset = false,
                             CharSet charset = CharSet::CP1250) {
    FileSettings result;
    result.type = csvOld;
    result.pathToFile = path;
    result.delimiter = delimiter;
    result.useCharset = useCharset;
    result.charset = charset;
    return result;
  }
};

std::shared_ptr<DataSets::MemoryDataSet> createDataSetFromFile(const std::string &name,
                                                               const FileSettings &fileSettings,
                                                               const std::vector<ValueType> &valueTypes) {
  std::unique_ptr<DataProviders::BaseDataProvider> provider;
  switch (fileSettings.type) {
  case FileSettings::csv:
    if (fileSettings.useCharset) {
      provider = std::make_unique<DataProviders::CsvStreamReader>(fileSettings.pathToFile, fileSettings.charset,
                                                                  fileSettings.delimiter);
    } else {
      provider = std::make_unique<DataProviders::CsvStreamReader>(fileSettings.pathToFile, fileSettings.delimiter);
    }
    break;
  case FileSettings::csvOld:
    if (fileSettings.useCharset) {
      provider = std::make_unique<DataProviders::CsvReader>(fileSettings.pathToFile, fileSettings.charset,
                                                            std::string(1, fileSettings.delimiter), true);
    } else {
      provider = std::make_unique<DataProviders::CsvReader>(fileSettings.pathToFile,
                                                            std::string(1, fileSettings.delimiter), true);
    }
    break;
  case FileSettings::xlsx:
    provider = std::make_unique<DataProviders::XlsxIOReader>(fileSettings.pathToFile);
    if (!fileSettings.sheet.empty()) {
      dynamic_cast<DataProviders::XlsxIOReader *>(provider.get())->openSheet(fileSettings.sheet);
    }
    break;
  case FileSettings::xls:
    provider = std::make_unique<DataProviders::XlsxReader>(fileSettings.pathToFile);
    break;
  }
  auto result = std::make_shared<DataSets::MemoryDataSet>(name);
  result->open(*provider, valueTypes);
  logger.log<LogLevel::Status>("Loaded: "s + result->getName());
  result->resetEnd();
  logger.log<LogLevel::Debug, true>("Count:", result->getCurrentRecord());
  result->resetBegin();
  return result;
}
int main() {
  DataBase::MemoryDataBase db("db");

  db.addTable(
      createDataSetFromFile("res", FileSettings::Csv("/home/petr/Desktop/muni/res/res.csv", ',', true, CharSet::CP1250),
                            {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                             ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                             ValueType::String, ValueType::String, ValueType::String}));

  db.addTable(createDataSetFromFile("vel_kod", FileSettings::Csv("/home/petr/Desktop/muni/velikost_kody.csv", ','),
                                    {ValueType::String, ValueType::String}));

  const auto query ="SELECT res.ICO, "
                    "res.Stav, "
"res.Nazev, "
"res.Statisticka_pravni_forma, "
"res.Datum_vzniku, "
"res.Datum_zaniku, "
"res.Adresa, "
"res.\"Kod_stat._pravni_formy\", "
"res.\"Institucionalni_sektor\", "
"res.\"Cinnosti_dle_CZ-NACE\", "
"vel_kod.nazev FROM res JOIN vel_kod on res.\"Velikostni_kategorie\" = vel_kod.kod;";

  auto result = db.execSimpleQuery(query, false, "resKat");

  db.addTable(result->dataSet->toDataSet());
  db.removeTable("res");
  db.removeTable("vel_kod");

  DataWriters::CsvWriter writer{"/home/petr/Desktop/out.csv"};
  auto r = db.tableByName("resKat")->dataSet;
  writer.writeHeader(r->getFieldNames());
  std::vector<std::string> row;
  auto fields = r->getFields();
  writer.setAddQuotes(true);
  while(r->next()) {
    std::transform(fields.begin(), fields.end(), std::back_inserter(row),
                   [](auto &field) { return field->getAsString(); });
    writer.writeRecord(row);
    row.clear();
  }

  return 0;
}