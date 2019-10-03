//
// Created by Petr Flajsingr on 13/11/2018.
//

#include <ArrayWriter.h>
#include <iostream>

void DataWriters::ArrayWriter::writeHeader(const std::vector<std::string> &header) {
  result.emplace_back(header);
}

void DataWriters::ArrayWriter::writeRecord(const std::vector<std::string> &record) {
  result.emplace_back(record);
}

void DataWriters::ArrayWriter::print() const {
  for (auto &vec : result) {
    for (auto &str : vec) {
      std::cout << str << ",\t";
    }
    std::cout << std::endl;
  }
}

const std::vector<std::vector<std::string>> &DataWriters::ArrayWriter::getArray() const {
  return result;
}
