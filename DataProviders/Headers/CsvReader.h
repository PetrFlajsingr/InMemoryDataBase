//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef DATAPROVIDERS_HEADERS_CSVREADER_H_
#define DATAPROVIDERS_HEADERS_CSVREADER_H_

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <BaseDataProvider.h>
#include <Exceptions.h>

namespace DataProviders {
/**
 * Usage:
 *  DataProviders::CsvReader csvReader(PATH_TO_FILE, DELIMITER);
 *
 *  auto header = csvReader.getHeader();
 *
 *  while (csvReader.next()) {
 *      auto currentRow = csvReader.getRow();
 *  }
 *
 * Usage with iterator:
 *  DataProviders::CsvReader csvReader(PATH_TO_FILE, DELIMITER);
 *
 *  auto header = csvReader.getHeader();
 *
 *  for (auto row : header) {
 *      // work with data...
 *  }
 *
 */
class CsvReader : public BaseDataProvider {
 public:
  /**
   * If the file can not be opened throws IOException.
   * @param filePath path to file
   * @param delimiter csv delimiter
   */
  explicit CsvReader(std::string_view filePath, std::string_view delimiter = ",");

  explicit CsvReader(std::string_view filePath, CharSet inputCharSet, std::string_view delimiter = ",");

  /**
   * Close file on deletion.
   */
  ~CsvReader() override;

  inline const std::vector<std::string> &getRow() const override {
    return currentRecord;
  }

  inline std::string getColumnName(unsigned int columnIndex) const override {
    return header.at(columnIndex);
  }

  bool next() override;

  void first() override;

  int getCurrentRecordNumber() const override {
    return currentRecordNumber;
  }

  inline const std::vector<std::string> &getHeader() const override {
    return header;
  }

  inline uint64_t getColumnCount() const override {
    return header.size();
  }

  bool eof() const override;

 private:
  void init(std::string_view filePath, std::string_view delimiter);
  /**
   * FSM states for csv parsing.
   */
  enum class ParseState { Read, QuotMark1, QuotMark2 };

  const uint32_t BUFFER_SIZE = 65536;

  std::string delimiter;

  std::ifstream file;

  std::vector<std::string> header;

  std::vector<std::string> currentRecord;

  int currentRecordNumber = -1;

  bool _eof = false;

  /**
   * Read first row of the file into header member
   */
  void readHeader();

  /**
   * Read next row into currentRecord member
   */
  void parseRecord();

  /**
   * Parse line based on CSV specification.
   * @param line string to parse
   * @param vectorReserve value to reserve vector size for
   * @return tokenized vector from line
   */
  std::vector<std::string> tokenize(std::string_view line, unsigned int vectorReserve) const;

};
}  // namespace DataProviders

#endif  // DATAPROVIDERS_HEADERS_CSVREADER_H_
