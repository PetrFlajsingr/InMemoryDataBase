//
// Created by Petr Flajsingr on 13/11/2018.
//

#ifndef CSV_READER_BASEDATAWRITER_H
#define CSV_READER_BASEDATAWRITER_H

#include <gsl/gsl>
#include <string>
#include <vector>
#include <iterator>

namespace DataWriters {

/**
 * Rozhrani pro zapis dat.
 * Pouziti:
 * DataProviders::BaseDataProvider provider;
 *    while (!provider->eof()) {
 *       writer->writeRecord(provider->getRow());
 *       provider->next();
 *    }
 *
 * Pouziti s iteratorem:
 *    DataProviders::BaseDataProvider provider;
 *    auto iter = dataWriter->begin();
 *    for (auto row : provider) {
 *       *iter++ = row;
 *    }
 */
class BaseDataWriter {
 public:
  /**
   * Iterator pro jednoduchy zapis a vyuziti std funkci
   */
  class iterator : public std::iterator<std::output_iterator_tag,
                                        std::vector<std::string>> {
   private:
    BaseDataWriter *dataWriter;
   public:
    /**
     * Vytvoreni iteratoru pro dany data writer
     * @param dataWriter vystup nad kterym se iteruje
     */
    explicit iterator(gsl::not_null<BaseDataWriter *> dataWriter) : dataWriter(
        dataWriter) {}

    iterator(const iterator &other) {
      dataWriter = other.dataWriter;
    }

    /**
     * Zapis data z vector
     * @param rhs
     * @return
     */
    iterator &operator=(const std::vector<std::string> &rhs) {
      dataWriter->writeRecord(rhs);
      return *this;
    }

    /**
     * Nebylo nutne implementovat jinak
     * @return
     */
    iterator &operator*() {
      return *this;
    }

    /**
     * Nebylo nutne implementovat jinak
     * @return
     */
    iterator &operator++() {
      return *this;
    }

    /**
     * Nebylo nutne implementovat jinak
     * @return
     */
    const iterator operator++(int) {
      return *this;
    }

  };

  /**
   * Zapise hlavicku dat (nazvy sloupcu)
   * @param header nazvy sloupcu
   */
  virtual void writeHeader(const std::vector<std::string> &header) = 0;

  /**
   * Zapise zaznam
   * @param record zaznam k zapsani
   */
  virtual void writeRecord(const std::vector<std::string> &record) = 0;

  iterator begin() {
    return iterator(this);
  }

  virtual ~BaseDataWriter() = default;
};

}  // namespace DataWriters

#endif //CSV_READER_BASEDATAWRITER_H
