//
// Created by Petr Flajsingr on 30/08/2018.
//

#ifndef DATAPROVIDERS_HEADERS_ARRAYDATAPROVIDER_H_
#define DATAPROVIDERS_HEADERS_ARRAYDATAPROVIDER_H_

#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include <BaseDataProvider.h>

namespace DataProviders {

/**
 * Provider data from vector.
 */
class ArrayDataProvider : public BaseDataProvider {
 public:
  /**
   * Prepares header names as indexes of fields.
   */
  explicit ArrayDataProvider(const std::vector<std::vector<std::string>> &data)
      : BaseDataProvider(), data(data) {
    header.reserve(data[0].size());
    for (gsl::index i = 0; i < data[0].size(); ++i) {
      header.emplace_back(std::to_string(i));
    }
  }
  const std::vector<std::string> &getRow() const override {
    return data[currentRow];
  }
  std::string getColumnName(unsigned int columnIndex) const override {
    return std::to_string(columnIndex);
  }
  uint64_t getColumnCount() const override {
    return data[0].size();
  }
  const std::vector<std::string> &getHeader() const override {
    return header;
  }
  int getCurrentRecordNumber() const override {
    return currentRow;
  }
  bool next() override {
    if (currentRow == -1 || currentRow < data.size() - 1) {
      currentRow++;
      return true;
    }
    _eof = true;
    return false;
  }
  void first() override {
    currentRow = -1;
    _eof = false;
  }
  bool eof() const override {
    return _eof;
  }

 private:
  std::vector<std::vector<std::string>> data;

  std::vector<std::string> header;

  int currentRow = -1;

  bool _eof = false;
};

}  // namespace DataProviders

#endif  // DATAPROVIDERS_HEADERS_ARRAYDATAPROVIDER_H_
