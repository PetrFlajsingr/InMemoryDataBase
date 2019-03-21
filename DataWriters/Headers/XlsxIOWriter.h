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

/**
 * Write data to xlsx file using XlsxIO library.
 */
class XlsxIOWriter : public BaseDataWriter {
 public:
  /**
   *
   * @param fileName destination
   * @param sheetName name of sheet to write
   */
  explicit XlsxIOWriter(std::string_view fileName, std::string_view sheetName = "");
  /**
   *
   * @param fileName destination
   * @param charSet output encoding
   * @param sheetName name of sheet to write
   */
  XlsxIOWriter(std::string_view fileName, CharSet charSet, std::string_view sheetName = "");
  ~XlsxIOWriter() override;
  void writeHeader(const std::vector<std::string> &header) override;
  void writeRecord(const std::vector<std::string> &record) override;

 private:
  xlsxiowriter xlsxWriterHandle;
};

}  // namespace DataWriters

#endif //CSV_READER_XLSXWRITER_H
