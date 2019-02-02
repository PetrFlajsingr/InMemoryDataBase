//
// Created by Petr Flajsingr on 2019-01-25.
//

#include <Exceptions.h>
#include "XlsxWriter.h"

DataWriters::XlsxWriter::XlsxWriter(std::string_view fileName,
                                    std::string_view sheetName) {
  xlsxWriterHandle = xlsxiowrite_open(std::string(fileName).c_str(),
                                      std::string(sheetName).c_str());

  if (xlsxWriterHandle == nullptr) {
    throw IOException("Error creating .xlsx file");
  }
}

DataWriters::XlsxWriter::~XlsxWriter() {
  xlsxiowrite_close(xlsxWriterHandle);
}

void DataWriters::XlsxWriter::writeHeader(const std::vector<std::string> &header) {
  for (auto &name : header) {
    xlsxiowrite_add_column(xlsxWriterHandle, name.c_str(), 0);
  }
  xlsxiowrite_next_row(xlsxWriterHandle);
}

void DataWriters::XlsxWriter::writeRecord(const std::vector<std::string> &record) {
  for (auto &value : record) {
    xlsxiowrite_add_cell_string(xlsxWriterHandle, value.c_str());
  }
  xlsxiowrite_next_row(xlsxWriterHandle);
}
