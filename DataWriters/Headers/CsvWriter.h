//
// Created by Petr Flajsingr on 13/10/2018.
//

#ifndef DATAWRITERS_HEADERS_CSVWRITER_H_
#define DATAWRITERS_HEADERS_CSVWRITER_H_

#include <string>
#include <vector>
#include <fstream>
#include "BaseDataWriter.h"

namespace DataWriters {

/**
 * Zapsani dat ve formatu csv na disk.
 */
class CsvWriter : public BaseDataWriter {
 private:
  std::ofstream *outputStream;

  uint64_t columnCount = 0;

  std::string delimiter;

  void writeRow(const std::vector<std::string> &data);
 public:
  explicit CsvWriter(std::string filePath, std::string delimiter = ";");

  ~CsvWriter() override;

  void writeHeader(const std::vector<std::string> &header) override;

  void writeRecord(const std::vector<std::string> &record) override;
};

}  // namespace DataWriters

#endif  //DATAWRITERS_HEADERS_CSVWRITER_H_
