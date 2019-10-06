//
// Created by Petr Flajsingr on 13/11/2018.
//

#ifndef CSV_READER_BASEDATAWRITER_H
#define CSV_READER_BASEDATAWRITER_H

#include <Converters.h>
#include <gsl/gsl>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

namespace DataWriters {

/**
 * Base class for data writers.
 */
class BaseDataWriter {
public:
  /**
   * Construct a data writer without encoding conversion.
   */
  BaseDataWriter() : convert(false) {}
  /**
   * Construct a data writer with encoding conversion from utf8.
   * @param charSet output encoding
   */
  explicit BaseDataWriter(CharSet charSet) : convert(true), converter(std::make_unique<CharSetConverter>(charSet)) {}
  /**
   * Write column names.
   * @param header column names
   */
  virtual void writeHeader(const std::vector<std::string> &header) = 0;
  /**
   * Write row.
   * @param record row to write
   */
  virtual void writeRecord(const std::vector<std::string> &record) = 0;
  virtual ~BaseDataWriter() = default;

  class iterator : public std::iterator<std::output_iterator_tag, std::vector<std::string>> {
  private:
    BaseDataWriter *dataWriter;

  public:
    explicit iterator(gsl::not_null<BaseDataWriter *> dataWriter) : dataWriter(dataWriter) {}
    iterator(const iterator &other) { dataWriter = other.dataWriter; }

    iterator &operator=(const iterator &rhs) {
      dataWriter = rhs.dataWriter;
      return *this;
    }
    /**
     * Write data to ouput.
     * @param rhs data to write
     */
    iterator &operator=(const std::vector<std::string> &rhs) {
      dataWriter->writeRecord(rhs);
      return *this;
    }
    iterator &operator*() { return *this; }
    iterator &operator++() { return *this; }

    iterator operator++(int) { return *this; }
  };

  iterator begin() { return iterator(this); }

protected:
  bool convert;
  std::unique_ptr<CharSetConverter> converter;
};

} // namespace DataWriters

#endif // CSV_READER_BASEDATAWRITER_H
