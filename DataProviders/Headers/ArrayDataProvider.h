//
// Created by Petr Flajsingr on 30/08/2018.
//

#ifndef DATAPROVIDERS_HEADERS_ARRAYDATAPROVIDER_H_
#define DATAPROVIDERS_HEADERS_ARRAYDATAPROVIDER_H_

#include <utility>
#include <vector>
#include <string>
#include <algorithm>
#include "BaseDataProvider.h"

namespace DataProviders {

/**
 * Class used for testing
 */
class ArrayDataProvider : public BaseDataProvider {
 public:
  /**
   * Prepares header names as indexes of fields.
   */
  explicit ArrayDataProvider(const std::vector<std::vector<std::string>> &data)
      : data(data) {
    header.reserve(data[0].size());
    for (auto i = 0; i < data[0].size(); ++i) {
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
    if (currentRow < data.size()) {
      currentRow++;
      return true;
    }
    return false;
  }

  void first() override {
    currentRow = 0;
  }

  bool eof() const override {
    return currentRow == data.size();
  }

 private:
  std::vector<std::vector<std::string>> data;

  std::vector<std::string> header;

  uint64_t currentRow = 0;

};

}  // namespace DataProviders

#endif  // DATAPROVIDERS_HEADERS_ARRAYDATAPROVIDER_H_
