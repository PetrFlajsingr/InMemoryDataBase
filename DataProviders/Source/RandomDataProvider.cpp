//
// Created by Petr Flajsingr on 2019-01-30.
//

#include <RandomDataProvider.h>
#include <Utilities.h>

DataProviders::RandomDataProvider::RandomDataProvider(int columnCount,
                                                      int minDataLength,
                                                      int maxDataLength,
                                                      int recordCount)
    : columnCount(columnCount), recordCount(recordCount) {
  std::random_device randomDevice;
  randomEngine = std::default_random_engine(randomDevice());
  uniformIntDistribution =
      std::uniform_int_distribution(minDataLength, maxDataLength);
  for (auto i = 0; i < columnCount; ++i) {
    header.emplace_back(Utilities::random_string(static_cast<size_t>(uniformIntDistribution(
        randomEngine))));
  }
}

const std::vector<std::string> &DataProviders::RandomDataProvider::getRow() const {
  return currentRecord;
}

std::string DataProviders::RandomDataProvider::getColumnName(unsigned int columnIndex) const {
  return header.at(columnIndex);
}

uint64_t DataProviders::RandomDataProvider::getColumnCount() const {
  return header.size();
}

const std::vector<std::string> &DataProviders::RandomDataProvider::getHeader() const {
  return header;
}

uint64_t DataProviders::RandomDataProvider::getCurrentRecordNumber() const {
  return currentRecordNumber;
}

bool DataProviders::RandomDataProvider::next() {
  if (currentRecordNumber == recordCount) {
    _eof = true;
    return false;
  }
  currentRecord.clear();
  for (auto i = 0; i < columnCount; ++i) {
    currentRecord.emplace_back(Utilities::random_string(static_cast<size_t>(uniformIntDistribution(
        randomEngine))));
  }
  currentRecordNumber++;
  return true;
}

void DataProviders::RandomDataProvider::first() {
  throw NotImplementedException();
}

bool DataProviders::RandomDataProvider::eof() const {
  return _eof;
}
