//
// Created by petr on 10/3/19.
//

#include "CsvStreamReader.h"

DataProviders::CsvStreamReader::CsvStreamReader(const std::string &filePath, char delimiter)
    : BaseDataProvider(), csvStream(std::make_unique<csvstream>(filePath, delimiter)), filePath(filePath),
      delimiter(delimiter) {
  header = csvStream->getheader();
  if (header.back().empty()) {
    header = std::vector<std::string>(header.begin(), header.end() - 1);
  }
  header = removeQuotes(header);
}

DataProviders::CsvStreamReader::CsvStreamReader(const std::string &filePath, CharSet inputCharSet, char delimiter)
    : BaseDataProvider(inputCharSet), csvStream(std::make_unique<csvstream>(filePath, delimiter)), filePath(filePath),
      delimiter(delimiter) {
  header = csvStream->getheader();
  if (header.back().empty()) {
    header = std::vector<std::string>(header.begin(), header.end() - 1);
  }
  header = removeQuotes(header);
}

bool DataProviders::CsvStreamReader::next() {
  if (!(*csvStream >> currentRecord)) {
    _eof = true;
    return false;
  }
  shrinkRecordToFit();
  currentRecord = removeQuotes(currentRecord);
  if (convert) {
    for (auto & i : currentRecord) {
      i = converter->convert(i);
    }
  }

  currentRecordNumber++;
  return true;
}

bool DataProviders::CsvStreamReader::eof() const { return _eof; }

void DataProviders::CsvStreamReader::first() {
  currentRecordNumber = -1;
  _eof = false;
  csvStream = std::make_unique<csvstream>(filePath, delimiter);
}

void DataProviders::CsvStreamReader::shrinkRecordToFit() {
  if (currentRecord.size() > header.size()) {
    auto lastValidRecordIndex = header.size() - 1;
    for (gsl::index i = lastValidRecordIndex + 1; i < static_cast<gsl::index>(currentRecord.size()); ++i) {
      currentRecord[lastValidRecordIndex] += currentRecord[i];
    }
    currentRecord = std::vector<std::string>(currentRecord.begin(), currentRecord.begin() + lastValidRecordIndex + 1);
  }
}
std::vector<std::string> DataProviders::CsvStreamReader::removeQuotes(const std::vector<std::string> &data) {
  auto result = data;
  for (auto i = result.begin(); i != result.end(); ++i) {
    std::size_t offsetStart = 0;
    std::size_t offsetEnd = i->length();
    if ((*i)[0] == '"') {
      offsetStart = 1;
    }
    if ((*i)[i->length() - 1] == '"') {
      offsetEnd = i->length() - 1 - offsetStart;
    }
    *i = i->substr(offsetStart, offsetEnd);
    auto beg = i->begin();
    auto end = i->end();
    if (std::distance(beg, end) < 2) {
      continue;
    }
    while (beg != end - 1) {
      if (*beg == '"' && *(beg + 1) == '"') {
        beg = i->erase(beg);
      } else {
        ++beg;
      }
    }
  }
  return result;
}
