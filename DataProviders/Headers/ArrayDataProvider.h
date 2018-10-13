//
// Created by Petr Flajsingr on 30/08/2018.
//

#ifndef DATAPROVIDERS_HEADERS_ARRAYDATAPROVIDER_H_
#define DATAPROVIDERS_HEADERS_ARRAYDATAPROVIDER_H_

#include <utility>
#include <vector>
#include <string>
#include "BaseDataProvider.h"

namespace DataProviders {
/**
 * Dodavatel dat z pole a podobnych konstruktu.
 *
 * Podporuje jen dvojity vector, slouzi k testovani.
 */
class ArrayDataProvider : public BaseDataProvider {
 private:
  std::vector<std::vector<std::string>> data;

  uint64_t row = 0;

 public:
  /**
   * Kopiruje predana data
   */
  explicit ArrayDataProvider(const std::vector<std::vector<std::string>> &data)
    : data(data) {}

  ~ArrayDataProvider() override = default;

  /**
   * @return Momentalni pozice v datech
   */
  std::vector<std::string> getRow() override {
    std::vector<std::string> result;

    for (uint64_t i = 0; i < data[0].size(); ++i) {
      result.push_back(data[row][i]);
    }

    return result;
  }

  std::string getColumn(unsigned int columnIndex) override {
    return std::to_string(columnIndex);
  }

  uint64_t getColumnCount() override {
    return data[0].size();
  }

  std::vector<std::string> getColumnNames() override {
    std::vector<std::string> result;
    for (uint64_t i = 0; i < data[0].size(); ++i) {
      result.push_back(std::to_string(i));
    }

    return result;
  }

  uint64_t getCurrentRecordNumber() override {
    return row;
  }

  bool next() override {
    if (row < data.size()) {
      row++;
      return true;
    }
    return false;
  }

  bool previous() override {
    if (row > 0) {
      row--;
      return true;
    }
    return false;
  }

  void first() override {
    row = 0;
  }

  void last() override {
    row = data.size() - 1;
  }

  bool eof() override {
    return row == data.size();
  }
};
}  // namespace DataProviders

#endif  // DATAPROVIDERS_HEADERS_ARRAYDATAPROVIDER_H_
