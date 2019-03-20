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

// TODO: "test,,,,,test"...
/**
 * Write csv data to file.
 */
class CsvWriter : public BaseDataWriter {
 public:
  /**
   *
   * @param filePath destination
   * @param delimiter csv delimiter
   */
  explicit CsvWriter(std::string_view filePath,
                     std::string_view delimiter = ",");
  /**
   *
   * @param filePath destination
   * @param charSet output encoding
   * @param delimiter csv delimiter
   */
  CsvWriter(std::string_view filePath,
            CharSet charSet,
            std::string_view delimiter = ",");
  ~CsvWriter() override;
  void writeHeader(const std::vector<std::string> &header) override;
  void writeRecord(const std::vector<std::string> &record) override;

 private:
  std::ofstream *outputStream;
  uint64_t columnCount = 0;
  std::string delimiter;
  /**
   * Write row of delimited data to outputStream
   * @param data data to write
   */
  void writeRow(const std::vector<std::string> &data);
};

}  // namespace DataWriters

#endif  //DATAWRITERS_HEADERS_CSVWRITER_H_
