//
// Created by Petr Flajsingr on 2019-01-25.
//

#ifndef CSV_READER_XLSXWRITER_H
#define CSV_READER_XLSXWRITER_H

#include <xlsxio_write.h>
#include <vector>
#include <string>
#include "BaseDataWriter.h"

namespace DataWriters {

class XlsxWriter : public BaseDataWriter {
 public:
  explicit XlsxWriter(std::string_view fileName,
                      std::string_view sheetName = "");

  XlsxWriter(std::string_view fileName,
             CharSetConverter::CharSet outCharSet,
             std::string_view sheetName = "");

  ~XlsxWriter() override;

  void writeHeader(const std::vector<std::string> &header) override;

  void writeRecord(const std::vector<std::string> &record) override;

 private:
  xlsxiowriter xlsxWriterHandle;
};

}  // namespace DataWriters

#endif //CSV_READER_XLSXWRITER_H
