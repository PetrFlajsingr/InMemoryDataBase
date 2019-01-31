//
// Created by Petr Flajsingr on 2019-01-30.
//

#ifndef PROJECT_RANDOMDATAPROVIDER_H
#define PROJECT_RANDOMDATAPROVIDER_H

#include <string>
#include <vector>
#include <BaseDataProvider.h>
#include <random>
#include <Utilities.h>
#include <gsl/gsl>

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
  RandomDataProvider(int columnCount,
                     int minDataLength,
                     int maxDataLength,
                     int recordCount);

  const std::vector<std::string> &getRow() const override;

  std::string getColumnName(unsigned int columnIndex) const override;

  uint64_t getColumnCount() const override;

  const std::vector<std::string> &getHeader() const override;

  int getCurrentRecordNumber() const override;

  bool next() override;

  void first() override;

  bool eof() const override;

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

}

#endif //PROJECT_RANDOMDATAPROVIDER_H
