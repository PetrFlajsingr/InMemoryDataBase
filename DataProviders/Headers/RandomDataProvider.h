//
// Created by Petr Flajsingr on 2019-01-30.
//

#ifndef PROJECT_RANDOMDATAPROVIDER_H
#define PROJECT_RANDOMDATAPROVIDER_H

#include <BaseDataProvider.h>
#include <Utilities.h>
#include <gsl/gsl>
#include <random>
#include <string>
#include <vector>

namespace DataProviders {

/**
 * Provide random data whose length is based on constructor parameters.
 */
class RandomDataProvider : public BaseDataProvider {
public:
  /**
   *
   * @param columnCount amount of records in each row
   * @param minDataLength minimum length of a cell
   * @param maxDataLength maximum length of a cell
   * @param recordCount amount of records in total
   */
  RandomDataProvider(int columnCount, int minDataLength, int maxDataLength, int recordCount);

  [[nodiscard]] const std::vector<std::string> &getRow() const override;

  [[nodiscard]] std::string getColumnName(unsigned int columnIndex) const override;

  [[nodiscard]] uint64_t getColumnCount() const override;

  [[nodiscard]] const std::vector<std::string> &getHeader() const override;

  [[nodiscard]] int getCurrentRecordNumber() const override;

  bool next() override;

  void first() override;

  [[nodiscard]] bool eof() const override;

private:
  int columnCount;

  const int recordCount;

  int currentRecordNumber = -1;

  bool _eof = false;

  std::vector<std::string> header;

  std::vector<std::string> currentRecord;

  std::uniform_int_distribution<int> uniformIntDistribution;

  std::default_random_engine randomEngine;
};

} // namespace DataProviders

#endif // PROJECT_RANDOMDATAPROVIDER_H
