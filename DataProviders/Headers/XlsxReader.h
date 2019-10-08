//
// Created by petr on 10/8/19.
//

#ifndef CSV_READER_XLSREADER_H
#define CSV_READER_XLSREADER_H

#include "BaseDataProvider.h"
#include <XlsReader.h>

namespace DataProviders {
class XlsxReader : public BaseDataProvider {
public:
  explicit XlsxReader(std::string_view path);
  [[nodiscard]] const std::vector<std::string> &getRow() const override;

  [[nodiscard]] std::string getColumnName(unsigned int columnIndex) const override;

  [[nodiscard]] uint64_t getColumnCount() const override;

  [[nodiscard]] const std::vector<std::string> &getHeader() const override;

  [[nodiscard]] int getCurrentRecordNumber() const override;

  bool next() override;

  void first() override;

  [[nodiscard]] bool eof() const override;

private:
  xls::WorkBook workBook;
};
}

#endif // CSV_READER_XLSREADER_H
