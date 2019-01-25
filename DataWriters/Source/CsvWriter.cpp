//
// Created by Petr Flajsingr on 13/10/2018.
//

#include <Exceptions.h>
#include <vector>
#include "CsvWriter.h"

CsvWriter::CsvWriter(std::string filePath, std::string delimiter) {
  this->delimiter = delimiter;
  outputStream = new std::ofstream();
  outputStream->open(filePath, std::ofstream::out);
}

CsvWriter::~CsvWriter() {
  outputStream->close();
  delete outputStream;
}
void CsvWriter::writeHeader(const std::vector<std::string> &header) {
  if (header.empty()) {
    throw InvalidArgumentException("Header can not be empty.");
  }

  columnCount = header.size();

  writeRow(header);
}
void CsvWriter::writeRecord(const std::vector<std::string> &record) {
  if (record.size() != columnCount) {
    throw InvalidArgumentException("Length of record doesn't match length of header.");
  }

  writeRow(record);
}
void CsvWriter::writeRow(const std::vector<std::string> &data) {
  for (auto i = 0; i < columnCount - 1; ++i) {
    *outputStream << data[i] << delimiter;
  }
  *outputStream << data[columnCount - 1] << "\n";
}
