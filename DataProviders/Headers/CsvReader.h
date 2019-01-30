//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef DATAPROVIDERS_HEADERS_CSVREADER_H_
#define DATAPROVIDERS_HEADERS_CSVREADER_H_

#include <fstream>
#include <vector>
#include <string>
#include "BaseDataProvider.h"
#include "Exceptions.h"

namespace DataProviders {
/**
 * Trida implementujici metody IReader.
 * Slouzi ke cteni CSV formatu sekvencne ze souboru.
 *
 * Podporuje libovolny delimiter, predpoklada \n nebo \r\n pro konec zaznamu.
 * Podporuje uvozovky " pro pouziti delimiteru v v zaznamu.
 *
 * Ukazka pouziti:
 *  auto csvReader = new DataProviders::CsvReader(PATH_TO_FILE, DELIMITER);
 *
 *  auto header = csvReader->getHeader();
 *
 *  while (!csvReader->eof()) {
 *      auto currentRow = csvReader->getRow();
 *
 *      csvReader->next();
 *  }
 *
 *  delete csvReader;
 * Pouziti s iterator:
 *  auto csvReader = new DataProvider::CsvReader(PATH_TO_FILe, DELIMITER);
 *
 *  auto header = csvReader->getHeader();
 *
 *  for (auto row : *header) {
 *      //delej neco se zaznamem
 *  }
 *
 */
class CsvReader : public BaseDataProvider {
 private:
  enum TokeniserStates { Read, QuotMark1, QuotMark2 };
  const uint32_t BUFFER_SIZE = 4096;  //< velikost bufferu pro cteni radku

  std::string delimiter;  //< rozdelovac CSV dat

  std::ifstream file;  //< vstupni soubor

  std::vector<std::string> header;  //< nazvy sloupcu zaznamu

  std::vector<std::string> currentRecord;  //< momentalni zaznam, rozdeleny

  uint64_t currentRecordNumber = 0;  //< cislo momentalniho zaznamu

  /**
   * Precteni hlavicky CSV souboru a zapsani do this->header
   */
  void readHeader();

  /**
   * Zpracovani zaznamu pred jeho zpristupnenim
   */
  void parseRecord();

  std::vector<std::string> tokenize(const std::string &line,
                                    int vectorReserve) const;

 public:
  /**
   * Vytvori ctecku nad zadanym souborem.
   *
   * Pokud se soubor nepodari otevrit je vyhozena IOException.
   * @param filePath Cesta k souboru
   */
  explicit CsvReader(std::string filePath, std::string delimiter = ",");

  /**
   * Uzavreni souboru pri dealokaci objektu.
   */
  ~CsvReader() override;

  /**
   * Vraci zaznam souboru, ktery je na rade.
   * @return rozdeleny zaznamu podle CSV delimiter.
   */
  inline const std::vector<std::string> &getRow() const override {
    return currentRecord;
  }

  inline std::string getColumnName(unsigned int columnIndex) const override {
    return header.at(columnIndex);
  }

  /**
   * Posun na dalsi zaznam.
   * @return false pokud byl přečten celý soubor (EOF), jinak true
   */
  bool next() override;

  void first() override;

  uint64_t getCurrentRecordNumber() const override {
    return currentRecordNumber;
  }

  inline const std::vector<std::string> &getHeader() const override {
    return header;
  }

  inline uint64_t getColumnCount() const override {
    return header.size();
  }

  bool eof() const override;

  /**
   * Nataveni delimiteru pro rozdelovani csv souboru
   * @param delimiter
   */
  void setDelimiter(char delimiter);
};
}  // namespace DataProviders

#endif  // DATAPROVIDERS_HEADERS_CSVREADER_H_
