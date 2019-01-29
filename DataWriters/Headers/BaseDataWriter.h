//
// Created by Petr Flajsingr on 13/11/2018.
//

#ifndef CSV_READER_BASEDATAWRITER_H
#define CSV_READER_BASEDATAWRITER_H

#include <string>
#include <vector>

namespace DataWriters {

/**
 * Rozhrani pro zapis dat.
 */
class BaseDataWriter {
 public:
  class iterator : public std::iterator<std::output_iterator_tag,
                                        std::vector<std::string>> {
   private:
    BaseDataWriter *dataWriter;
   public:
    explicit iterator(BaseDataWriter *dataWriter) : dataWriter(dataWriter) {}

    iterator(const iterator &other) {
      dataWriter = other.dataWriter;
    }

    iterator &operator=(const std::vector<std::string> &rhs) {
      dataWriter->writeRecord(rhs);
      return *this;
    }

    iterator &operator*() {
      return *this;
    }

    iterator operator++(int) {
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
