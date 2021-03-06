//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef DATAPROVIDERS_HEADERS_BASEDATAPROVIDER_H_
#define DATAPROVIDERS_HEADERS_BASEDATAPROVIDER_H_

#include <Converters.h>
#include <gsl/gsl>
#include <ostream>
#include <string>
#include <vector>

namespace DataProviders {

/**
 * Simple interface for reading a moving forward in records.
 */
class BaseDataProvider {
public:
  BaseDataProvider() : convert(false) {}
  /**
   *
   * @param charSet source charset
   */
  explicit BaseDataProvider(CharSet charSet) : convert(true), converter(std::make_unique<CharSetConverter>(charSet)) {}
  /**
   * Get a record divided into fields
   * @return
   */
  [[nodiscard]] virtual const std::vector<std::string> &getRow() const = 0;

  /**
   * Get a name of a column - usually first row in a file...
   * @param columnIndex Index of desired column name
   * @return name of the column
   */
  [[nodiscard]] virtual std::string getColumnName(unsigned int columnIndex) const = 0;

  [[nodiscard]] virtual uint64_t getColumnCount() const = 0;

  /**
   *
   * @return vector of all column names
   */
  [[nodiscard]] virtual const std::vector<std::string> &getHeader() const = 0;

  /**
   *
   * @return amount of records that have been read already
   */
  [[nodiscard]] virtual int getCurrentRecordNumber() const = 0;

  /**
   * Move to the next record.
   * @return true if function call produced new record, false otherwise
   */
  virtual bool next() = 0;

  /**
   * Move to the first record
   */
  virtual void first() = 0;

  [[nodiscard]] virtual bool eof() const = 0;

  virtual ~BaseDataProvider() = default;

  class iterator : public std::iterator<std::input_iterator_tag, std::vector<std::string>> {
  private:
    BaseDataProvider *provider;

  public:
    /**
     * Move provider to the first record when no data has been read.
     * @param provider
     */
    explicit iterator(gsl::not_null<BaseDataProvider *> provider) : provider(provider) {
      if (provider->getCurrentRecordNumber() == -1) {
        provider->next();
      }
    }

    iterator(const iterator &other) { provider = other.provider; }

    iterator &operator=(const iterator &other) {
      provider = other.provider;
      return *this;
    }

    /**
     * Move to next record
     * @return
     */
    iterator &operator++() {
      provider->next();
      return *this;
    }

    iterator operator++(int) {
      iterator result = *this;
      ++(*this);
      return result;
    }

    /**
     * Abuse of == for eof checking
     * @return
     */
    bool operator==(const iterator &) const { return provider->eof(); }

    bool operator!=(const iterator &other) const { return !(*this == other); }

    /**
     * Get current record from provider
     * @return
     */
    const std::vector<std::string> &operator*() const { return provider->getRow(); }
  };

  iterator begin() { return iterator(this); }
  iterator end() { return iterator(this); }

  friend std::ostream &operator<<(std::ostream &os, BaseDataProvider &provider) {
    provider.next();
    for (const auto &val : provider.getRow()) {
      os << val << " ";
    }
    return os;
  }

protected:
  bool convert;
  std::unique_ptr<CharSetConverter> converter;

};

} // namespace DataProviders

#endif //  DATAPROVIDERS_HEADERS_BASEDATAPROVIDER_H_
