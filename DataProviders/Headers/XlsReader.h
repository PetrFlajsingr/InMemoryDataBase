//
// Created by petr on 10/8/19.
//

#ifndef CSV_READER_XLSREADER_H
#define CSV_READER_XLSREADER_H

#include "BaseDataProvider.h"

namespace DataProviders {
class XlsReader : public BaseDataProvider {
public:
  const std::vector<std::string> &getRow() const override;
  std::string getColumnName(unsigned int columnIndex) const override;
  uint64_t getColumnCount() const override;
  const std::vector<std::string> &getHeader() const override;
  int getCurrentRecordNumber() const override;
  bool next() override;
  void first() override;
  bool eof() const override;
};
}

#endif // CSV_READER_XLSREADER_H
