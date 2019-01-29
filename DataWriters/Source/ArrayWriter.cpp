//
// Created by Petr Flajsingr on 13/11/2018.
//

#include <ArrayWriter.h>
#include <iostream>

#include "ArrayWriter.h"
void DataWriters::ArrayWriter::writeHeader(const std::vector<std::string> &header) {
  result->push_back(header);
}

void DataWriters::ArrayWriter::writeRecord(const std::vector<std::string> &record) {
  result->push_back(record);
}

void DataWriters::ArrayWriter::print() {
  for (auto &vec : *result) {
    for (auto &str : vec) {
      std::cout << str << ",\t";
    }
    std::cout << std::endl;
  }
}

std::vector<std::vector<std::string>> *DataWriters::ArrayWriter::getArray() {
  return result;
}
