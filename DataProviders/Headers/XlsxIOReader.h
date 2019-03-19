//
// Created by Petr Flajsingr on 2019-01-28.
//

#ifndef CSV_READER_XLSREADER_H
#define CSV_READER_XLSREADER_H

#include <xlsxio_read.h>
#include <gsl/gsl>
#include <string>
#include <vector>
#include <BaseDataProvider.h>
#include <Exceptions.h>

namespace DataProviders {

class XlsxIOReader : public BaseDataProvider {
 public:
  explicit XlsxIOReader(std::string_view fileName);
  XlsxIOReader(std::string_view fileName,
               CharSetConverter::CharSet inputCharSet);

  ~XlsxIOReader() override;

  const std::vector<std::string> &getRow() const override;

  std::string getColumnName(unsigned int columnIndex) const override;

  uint64_t getColumnCount() const override;

  const std::vector<std::string> &getHeader() const override;

  int getCurrentRecordNumber() const override;

  bool next() override;

  void first() override;

  bool eof() const override;

 private:
  void init(std::string_view fileName);

  xlsxioreader xlsxioReader;

  xlsxioreadersheet xlsxioSheet;

  std::vector<std::string> currentRecord;

  std::vector<std::string> header;

  int currentRecordNumber = -1;

  bool _eof = false;

  /**
   * Close input file.
   */
  void close();

  /**
   * Read first row from the file and save it to header member.
   */
  void readHeader();
};

}  // namespace DataProviders

#endif //CSV_READER_XLSREADER_H
