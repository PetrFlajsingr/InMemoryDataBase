//
// Created by Petr Flajsingr on 13/11/2018.
//

#include "ArrayWriter.h"
void ArrayWriter::writeHeader(const std::vector<std::string> &header) {
  for (auto &str : header) {
    result->push_back(str);
  }
}

void ArrayWriter::writeRecord(const std::vector<std::string> &record) {
  for (auto &str : record) {
    result->push_back(str);
  }
}
