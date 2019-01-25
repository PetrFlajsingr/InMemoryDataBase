//
// Created by Petr Flajsingr on 13/10/2018.
//

#ifndef DATAWRITERS_HEADERS_CSVWRITER_H_
#define DATAWRITERS_HEADERS_CSVWRITER_H_

#include <string>
#include <fstream>
#include "BaseDataWriter.h"

class CsvWriter : public BaseDataWriter {
 private:
  std::ofstream *outputStream;

  uint64_t columnCount = 0;

  std::string delimiter;

  void writeRow(const std::vector<std::string> &data);
 public:
  CsvWriter(std::string filePath, std::string delimiter = ";");

  virtual ~CsvWriter();

  void writeHeader(const std::vector<std::string> &header) override;

  void writeRecord(const std::vector<std::string> &record) override;
};

#endif  //DATAWRITERS_HEADERS_CSVWRITER_H_
