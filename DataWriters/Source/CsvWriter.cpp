//
// Created by Petr Flajsingr on 13/10/2018.
//

#include <Exceptions.h>
#include <vector>
#include "CsvWriter.h"

DataWriters::CsvWriter::CsvWriter(std::string_view filePath, std::string_view delimiter) : BaseDataWriter() {
  this->delimiter = delimiter;
  outputStream = new std::ofstream();
  outputStream->open(std::string(filePath), std::ofstream::out);
}

DataWriters::CsvWriter::CsvWriter(std::string_view filePath, CharSet charSet, std::string_view delimiter)
    : BaseDataWriter(charSet) {
  this->delimiter = delimiter;
  outputStream = new std::ofstream();
  outputStream->open(std::string(filePath), std::ofstream::out);
}

DataWriters::CsvWriter::~CsvWriter() {
  outputStream->close();
  delete outputStream;
}

void DataWriters::CsvWriter::writeHeader(const std::vector<std::string> &header) {
  if (header.empty()) {
    throw InvalidArgumentException("Headers can not be empty.");
  }
  columnCount = header.size();
  writeRow(header);
}

void DataWriters::CsvWriter::writeRecord(const std::vector<std::string> &record) {
  if (record.size() != columnCount) {
    throw InvalidArgumentException(
        "Length of record doesn't match length of header.");
  }
  writeRow(record);
}

void DataWriters::CsvWriter::writeRow(const std::vector<std::string> &data) {
  for (auto i = 0; i < columnCount - 1; ++i) {
    if (convert) {
      *outputStream << converter->convertBack(data[i]) << delimiter;
    } else {
      *outputStream << data[i] << delimiter;
    }
  }
  if (convert) {
    *outputStream << converter->convertBack(data[columnCount - 1]) << "\n";
  } else {
    *outputStream << data[columnCount - 1] << "\n";
  }
}

