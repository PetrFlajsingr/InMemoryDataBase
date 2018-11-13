//
// Created by Petr Flajsingr on 13/11/2018.
//

#include "ArrayWriter.h"
void ArrayWriter::writeHeader(const std::vector<std::string> &header) {
  result->push_back(header);
}

void ArrayWriter::writeRecord(const std::vector<std::string> &record) {
  result->push_back(record);
}
