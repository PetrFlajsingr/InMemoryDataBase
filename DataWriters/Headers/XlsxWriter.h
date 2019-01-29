//
// Created by Petr Flajsingr on 2019-01-25.
//

#ifndef CSV_READER_XLSXWRITER_H
#define CSV_READER_XLSXWRITER_H

#include "BaseDataWriter.h"
#include <xlsxio_write.h>

namespace DataWriters {
class XlsxWriter : public BaseDataWriter {
 private:
  xlsxiowriter xlsxWriterHandle;

 public:
  XlsxWriter(const std::string &fileName, const std::string &sheetName = "");

  ~XlsxWriter() override;

  void writeHeader(const std::vector<std::string> &header) override;

  void writeRecord(const std::vector<std::string> &record) override;
};

}  // namespace DataWriters

#endif //CSV_READER_XLSXWRITER_H
