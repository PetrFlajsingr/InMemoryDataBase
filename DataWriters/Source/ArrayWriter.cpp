//
// Created by Petr Flajsingr on 13/11/2018.
//

#include <ArrayWriter.h>
#include <iostream>

#include "ArrayWriter.h"
void ArrayWriter::writeHeader(const std::vector<std::string> &header) {
  result->push_back(header);
}

void ArrayWriter::writeRecord(const std::vector<std::string> &record) {
  result->push_back(record);
}
void ArrayWriter::print() {
  for (auto &vec : *result) {
    for (auto &str : vec) {
      std::cout << str << ",";
    }
    std::cout << std::endl;
  }
}
