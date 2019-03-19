//
// Created by Petr Flajsingr on 2019-03-19.
//

#include <Exceptions.h>
#include <XlntWriter.h>

#include "XlntWriter.h"

DataWriters::XlntWriter::XlntWriter(std::string_view fileName) {
  throw NotImplementedException();
}
void DataWriters::XlntWriter::writeHeader(const std::vector<std::string> &header) {
  throw NotImplementedException();
}
void DataWriters::XlntWriter::writeRecord(const std::vector<std::string> &record) {
  throw NotImplementedException();
}
