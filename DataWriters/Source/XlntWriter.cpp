//
// Created by Petr Flajsingr on 2019-03-19.
//

#include <Exceptions.h>
#include <XlntWriter.h>


DataWriters::XlntWriter::XlntWriter(std::string_view fileName) {
  ws = wb.active_sheet();
  destination = fileName;
}
void DataWriters::XlntWriter::writeHeader(const std::vector<std::string> &header) {
  for (gsl::index i = 0; i < header.size(); ++i) {
    ws.cell(i + 1, 1).value(header[i]);
  }
}
void DataWriters::XlntWriter::writeRecord(const std::vector<std::string> &record) {
  ++recordNumber;
  for (gsl::index i = 0; i < record.size(); ++i) {
    ws.cell(i + 1, recordNumber).value(record[i]);
  }
}
DataWriters::XlntWriter::~XlntWriter() {
  wb.save(destination);
}
