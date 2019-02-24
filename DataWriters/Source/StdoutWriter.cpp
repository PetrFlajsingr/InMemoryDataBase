//
// Created by Petr Flajsingr on 2019-02-23.
//

#include <Exceptions.h>
#include "StdoutWriter.h"
void DataWriters::StdoutWriter::writeHeader(const std::vector<std::string> &header) {
  std::for_each(header.begin(), header.end(), [](std::string_view str) {
    std::cout << str;
  });
  std::cout << "\n";
}
void DataWriters::StdoutWriter::writeRecord(const std::vector<std::string> &record) {
  std::for_each(record.begin(), record.end(), [](std::string_view str) {
    std::cout << str;
  });
  std::cout << "\n";
}
