//
// Created by Petr Flajsingr on 2019-03-19.
//

#ifndef PROJECT_XLNTREADER_H
#define PROJECT_XLNTREADER_H

#include "BaseDataProvider.h"
#include <xlnt/xlnt.hpp>

namespace DataProviders {
class XlntReader : public BaseDataProvider {
 public:
  explicit XlntReader(std::string_view fileName,
                      std::string_view sheetName = "");
  XlntReader(std::string_view fileName,
             CharSet inputCharSet,
             std::string_view sheetName = "");

  const std::vector<std::string> &getRow() const override;
  std::string getColumnName(unsigned int columnIndex) const override;
  uint64_t getColumnCount() const override;
  const std::vector<std::string> &getHeader() const override;
  int getCurrentRecordNumber() const override;
  bool next() override;
  void first() override;
  bool eof() const override;

 private:
  xlnt::streaming_workbook_reader wb;

  std::vector<std::string> header;
  std::vector<std::string> currentRecord;

  gsl::index currentRecordIndex = 0;

  std::string fileName;
  std::string sheetName;

  bool _eof = false;

  xlnt::cell lastUnusedCell;

  void readHeader();

  void readRecord();

  void prepareSheet();
};
}

#endif //PROJECT_XLNTREADER_H
