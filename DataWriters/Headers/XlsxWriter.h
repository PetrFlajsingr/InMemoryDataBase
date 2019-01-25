//
// Created by Petr Flajsingr on 2019-01-25.
//

#ifndef CSV_READER_XLSXWRITER_H
#define CSV_READER_XLSXWRITER_H

#include "BaseDataWriter.h"
#include <xlsxio_write.h>

class XlsxWriter : public BaseDataWriter {
 private:
  xlsxiowriter xlsxWriterHandle;

 public:
  XlsxWriter(std::string &fileName, std::string &sheetName);

  virtual ~XlsxWriter();

  void writeHeader(const std::vector<std::string> &header) override;
  void writeRecord(const std::vector<std::string> &record) override;
};

#endif //CSV_READER_XLSXWRITER_H
