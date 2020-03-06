//
// Created by petr on 3/6/20.
//

#ifndef CSV_READER_LOADINGUTILS_H
#define CSV_READER_LOADINGUTILS_H

#include <Converters.h>
#include <string>
#include "DataSets/Headers/MemoryDataSet.h"
struct FileSettings {
  enum Type { csv, xlsx, xls, csvOld };
  Type type;
  std::string pathToFile;
  char delimiter;
  std::string sheet = "";
  bool useCharset = false;
  CharSet charset;

  static FileSettings Xlsx(const std::string &path, const std::string &sheet = "");

  static FileSettings Xls(const std::string &path, const std::string &sheet = "");

  static FileSettings Csv(const std::string &path, char delimiter = ',', bool useCharset = false,
                          CharSet charset = CharSet::CP1250);

  static FileSettings CsvOld(const std::string &path, char delimiter = ',', bool useCharset = false,
                             CharSet charset = CharSet::CP1250);
};


std::shared_ptr<DataSets::MemoryDataSet> createDataSetFromFile(const std::string &name,
                                                               const FileSettings &fileSettings,
                                                               const std::vector<ValueType> &valueTypes);


#endif // CSV_READER_LOADINGUTILS_H
