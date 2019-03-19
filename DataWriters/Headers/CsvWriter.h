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
 public:
  explicit CsvWriter(std::string_view filePath,
                     std::string_view delimiter = ",");

  CsvWriter(std::string_view filePath,
            CharSet outCharSet,
            std::string_view delimiter = ",");

  ~CsvWriter() override;

  void writeHeader(const std::vector<std::string> &header) override;

  void writeRecord(const std::vector<std::string> &record) override;

 private:
  std::ofstream *outputStream;

  uint64_t columnCount = 0;

  std::string delimiter;

  void writeRow(const std::vector<std::string> &data);
};

}  // namespace DataWriters

#endif  //DATAWRITERS_HEADERS_CSVWRITER_H_
