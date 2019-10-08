//
// Created by petr on 10/8/19.
//

#ifndef CSV_READER_XLSXREADER_H
#define CSV_READER_XLSXREADER_H

#include "BaseDataProvider.h"
#include <xls.h>

namespace DataProviders {

class XlsxReader : public BaseDataProvider {
public:
  explicit XlsxReader(std::string path);
  ~XlsxReader() override;
  [[nodiscard]] const std::vector<std::string> &getRow() const override;

  [[nodiscard]] std::string getColumnName(unsigned int columnIndex) const override;

  [[nodiscard]] uint64_t getColumnCount() const override;

  [[nodiscard]] const std::vector<std::string> &getHeader() const override;

  [[nodiscard]] int getCurrentRecordNumber() const override;

  bool next() override;

  void first() override;

  [[nodiscard]] bool eof() const override;

private:
  xls::xlsWorkBook *workBook;
  xls::xlsWorkSheet *sheet;

  std::string filePath;

  gsl::index currentRecordNumber = 0;
  bool _eof = false;

  std::vector<std::string> currentRow;
  std::vector<std::string> header;

  void readHeader();

  void readRow(gsl::index rowNumber);

  void open();

  void close();
};
}

#endif // CSV_READER_XLSXREADER_H
