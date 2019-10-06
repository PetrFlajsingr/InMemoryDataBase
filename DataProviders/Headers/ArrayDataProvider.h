//
// Created by Petr Flajsingr on 30/08/2018.
//

#ifndef DATAPROVIDERS_HEADERS_ARRAYDATAPROVIDER_H_
#define DATAPROVIDERS_HEADERS_ARRAYDATAPROVIDER_H_

#include <BaseDataProvider.h>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace DataProviders {

/**
 * Provider data from vector.
 */
class ArrayDataProvider : public BaseDataProvider {
public:
  /**
   * Prepares header names as indexes of fields.
   */
  explicit ArrayDataProvider(const std::vector<std::vector<std::string>> &data) : BaseDataProvider() {
    this->data = std::vector<std::vector<std::string>>(data.begin() + 1, data.end());
    header.reserve(data[0].size());
    for (const auto &i : data[0]) {
      header.emplace_back(i);
    }
  }

  [[nodiscard]] const std::vector<std::string> &getRow() const override { return data[currentRow]; }

  [[nodiscard]] std::string getColumnName(unsigned int columnIndex) const override { return header[columnIndex]; }

  [[nodiscard]] uint64_t getColumnCount() const override { return data[0].size(); }

  [[nodiscard]] const std::vector<std::string> &getHeader() const override { return header; }

  [[nodiscard]] int getCurrentRecordNumber() const override { return currentRow; }
  bool next() override {
    if (currentRow == -1 || currentRow < static_cast<gsl::index>(data.size() - 1)) {
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

  [[nodiscard]] bool eof() const override { return _eof; }

private:
  std::vector<std::vector<std::string>> data;

  std::vector<std::string> header;

  gsl::index currentRow = -1;

  bool _eof = false;
};

} // namespace DataProviders

#endif // DATAPROVIDERS_HEADERS_ARRAYDATAPROVIDER_H_
