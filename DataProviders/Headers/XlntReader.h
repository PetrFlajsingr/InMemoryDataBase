//
// Created by Petr Flajsingr on 2019-03-19.
//

#ifndef PROJECT_XLNTREADER_H
#define PROJECT_XLNTREADER_H

#include "BaseDataProvider.h"
#include <string>
#include <vector>
#include <xlnt/xlnt.hpp>

namespace DataProviders {
class XlntReader : public BaseDataProvider {
public:
  explicit XlntReader(std::string_view fileName, std::string_view sheetName = "");
  XlntReader(std::string_view fileName, CharSet inputCharSet, std::string_view sheetName = "");

  [[nodiscard]] const std::vector<std::string> &getRow() const override;

  [[nodiscard]] std::string getColumnName(unsigned int columnIndex) const override;

  [[nodiscard]] uint64_t getColumnCount() const override;

  [[nodiscard]] const std::vector<std::string> &getHeader() const override;

  [[nodiscard]] int getCurrentRecordNumber() const override;
  bool next() override;
  void first() override;

  [[nodiscard]] bool eof() const override;

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
} // namespace DataProviders

#endif // PROJECT_XLNTREADER_H
