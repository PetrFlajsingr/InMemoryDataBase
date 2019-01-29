//
// Created by Petr Flajsingr on 2019-01-28.
//

#ifndef CSV_READER_XLSREADER_H
#define CSV_READER_XLSREADER_H

#include <string>
#include <vector>
#include <xlsxio_read.h>
#include "BaseDataProvider.h"

namespace DataProviders {

// TODO: napsat testy

/**
 * Trida pro cteni dat z xlsx souboru.
 *
 * Podporuje pouze cteni jako string.
 */
class XlsReader : BaseDataProvider {
 private:
  xlsxioreader xlsxioReader;  //< Handle na xlsxio reader

  xlsxioreadersheet xlsxioSheet;  //< Handle na sheet (pouziva se pouze prvni)

  std::vector<std::string> currentRecord;  //< Posledni rpecteny zaznam

  std::vector<std::string> header;  //< Nazvy sloupcu

  uint64_t currentRecordNumber = 0;

  bool _eof = false;

 public:
  /**
   * Otevri xlsx soubor fileName pro cteni
   * @param fileName
   */
  XlsReader(const std::string &fileName);

  ~XlsReader() override;

  std::vector<std::string> getRow() override;

  std::string getColumn(unsigned int columnIndex) override;

  uint64_t getColumnCount() override;

  std::vector<std::string> getHeader() override;

  uint64_t getCurrentRecordNumber() override;

  bool next() override;

  void first() override;

  bool eof() override;
};

}  // namespace DataProviders

#endif //CSV_READER_XLSREADER_H
