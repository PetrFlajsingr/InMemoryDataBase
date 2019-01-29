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
   * Iterator pro velmi snadnou iteraci zaznamy za pouziti for each konstrukce.
   */
  class iterator : public std::iterator<std::input_iterator_tag,
                                        std::vector<std::string>> {
   private:
    BaseDataProvider *provider;

   public:
    explicit iterator(BaseDataProvider *provider)
        : provider(provider) {}

    iterator(const iterator &other) {
      provider = other.provider;
    }

    /**
     * Posun na dalsi zaznam.
     * @return
     */
    iterator &operator++() {
      provider->next();
      return *this;
    }

    /**
     * Posun na dalsi zaznam
     * @return
     */
    const iterator operator++(int) {
      iterator result = *this;
      ++(*this);
      return result;
    }

    /**
     * Zneuziti porovnani begin() == end() pro kontrolu eof()
     * @return
     */
    bool operator==(const iterator &) const {
      return provider->eof();
    }

    /**
     * Zneuziti porovnani begin() == end() pro kontrolu eof()
     * @return
     */
    bool operator!=(const iterator &other) const {
      return !(*this == other);
    }

    /**
     * Dereference na aktualni zaznam
     * @return
     */
    std::vector<std::string> operator*() const {
      return provider->getRow();
    }
  };

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
   * Presun na prvni zaznam
   */
  virtual void first() = 0;

  /**
   * Kontrola dostupnosti zaznamu
   * @return false pokud neni dostupny dalsi zaznam, jinak true
   */
  inline virtual bool eof() = 0;

  iterator begin() {
    return iterator(this);
  }

  iterator end() {
    return iterator(this);
  }

  virtual ~BaseDataProvider() = default;
};

}  // namespace DataProviders

#endif //  DATAPROVIDERS_HEADERS_BASEDATAPROVIDER_H_
