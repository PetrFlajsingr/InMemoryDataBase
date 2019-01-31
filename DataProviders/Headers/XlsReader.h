//
// Created by Petr Flajsingr on 2019-01-28.
//

#ifndef CSV_READER_XLSREADER_H
#define CSV_READER_XLSREADER_H

#include <string>
#include <vector>
#include <xlsxio_read.h>
#include <BaseDataProvider.h>

namespace DataProviders {

class XlsReader : public BaseDataProvider {
 public:
  explicit XlsReader(const std::string &fileName);

  ~XlsReader() override;

  const std::vector<std::string> &getRow() const override;

  std::string getColumnName(unsigned int columnIndex) const override;

  uint64_t getColumnCount() const override;

  const std::vector<std::string> &getHeader() const override;

  int getCurrentRecordNumber() const override;

  bool next() override;

  void first() override;

  bool eof() const override;

 private:
  xlsxioreader xlsxioReader;

  xlsxioreadersheet xlsxioSheet;

  std::vector<std::string> currentRecord;

  std::vector<std::string> header;

  uint64_t currentRecordNumber = -1;

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
