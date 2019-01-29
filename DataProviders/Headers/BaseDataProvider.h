//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef DATAPROVIDERS_HEADERS_BASEDATAPROVIDER_H_
#define DATAPROVIDERS_HEADERS_BASEDATAPROVIDER_H_

#include <string>
#include <vector>

namespace DataProviders {
/**
 * Jednoduche rozhrani pro cteni a pohyb v zaznamech
 */
class BaseDataProvider {
 public:
  /**
   * Zaznam rozdeleny na sloupce
   * @return
   */
  virtual std::vector<std::string> getRow() = 0;

  /**
   * Hodnota ve vybranem sloupci
   * @param columnIndex Index sloupce
   * @return
   */
  virtual std::string getColumn(unsigned int columnIndex) = 0;

  /**
   * Pocet sloupcu zaznamu
   * @return
   */
  virtual uint64_t getColumnCount() = 0;

  /**
   * Nazvy sloupcu
   * @return
   */
  virtual std::vector<std::string> getHeader() = 0;

  /**
   * Pocet prozatim prectenych zaznamu
   * @return
   */
  virtual uint64_t getCurrentRecordNumber() = 0;

  /**
   * Presun na nasledujici zaznam
   * @return
   */
  virtual bool next() = 0;

  /**
   * Presun na predchozi zaznam
   * @return
   */
  virtual bool previous() = 0;

  /**
   * Presun na prvni zaznam
   */
  virtual void first() = 0;

  /**
   * Presun na posledni zaznam
   */
  virtual void last() = 0;

  /**
   * Kontrola dostupnosti zaznamu
   * @return false pokud neni dostupny dalsi zaznam, jinak true
   */
  inline virtual bool eof() = 0;

  virtual ~BaseDataProvider() = default;
};
}  // namespace DataProviders

#endif //  DATAPROVIDERS_HEADERS_BASEDATAPROVIDER_H_
