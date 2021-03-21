//
// Created by petr on 3/6/20.
//

#include "LoadingUtils.h"
#include <CsvReader.h>
#include <CsvStreamReader.h>
#include <XlsxIOReader.h>
#include <XlsxReader.h>
#include "io/logger.h"
using namespace LoggerStreamModifiers;
inline static Logger lgr{std::cout};
FileSettings FileSettings::Xlsx(const std::string &path, const std::string &sheet) {
  FileSettings result;
  result.type = xlsx;
  result.pathToFile = path;
  result.sheet = sheet;
  return result;
}
FileSettings FileSettings::Xls(const std::string &path, const std::string &sheet) {
  FileSettings result;
  result.type = xls;
  result.pathToFile = path;
  result.sheet = sheet;
  return result;
}
FileSettings FileSettings::CsvOld(const std::string &path, char delimiter, bool useCharset, CharSet charset) {
  FileSettings result;
  result.type = csvOld;
  result.pathToFile = path;
  result.delimiter = delimiter;
  result.useCharset = useCharset;
  result.charset = charset;
  return result;
}
FileSettings FileSettings::Csv(const std::string &path, char delimiter, bool useCharset, CharSet charset) {
  FileSettings result;
  result.type = csv;
  result.pathToFile = path;
  result.delimiter = delimiter;
  result.useCharset = useCharset;
  result.charset = charset;
  return result;
}
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
  lgr << status{} << fmt::format("Loaded dataset: '{}' from file: '{}'\n", result->getName(), fileSettings.pathToFile);
  result->resetEnd();
  lgr << debug{} << fmt::format("Line count: {}\n", result->getCurrentRecord()) << flush{};
  result->resetBegin();
  return result;
}
