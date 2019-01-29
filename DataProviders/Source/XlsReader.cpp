//
// Created by Petr Flajsingr on 2019-01-28.
//

#include <XlsReader.h>
#include <Exceptions.h>

DataProviders::XlsReader::XlsReader(const std::string &fileName) {
  xlsxioReader = xlsxioread_open(fileName.c_str());
  if (xlsxioReader == nullptr) {
    auto errMsg = "File could not be open: " + fileName;
    throw IOException(errMsg.c_str());
  }

  xlsxioSheet =
      xlsxioread_sheet_open(xlsxioReader, nullptr, XLSXIOREAD_SKIP_EMPTY_ROWS);

  if (xlsxioSheet == nullptr) {
    auto errMsg = "Error while opening xls file: " + fileName;
    throw IOException(errMsg.c_str());
  }

  if (xlsxioread_sheet_next_row(xlsxioSheet) != 0) {
    char *value;
    while ((value = xlsxioread_sheet_next_cell(xlsxioSheet)) != nullptr) {
      header.emplace_back(value);
      delete value;
    }
  }
}

std::vector<std::string> DataProviders::XlsReader::getRow() {
  return currentRecord;
}

std::string DataProviders::XlsReader::getColumn(unsigned int columnIndex) {
  return currentRecord.at(columnIndex);
}

uint64_t DataProviders::XlsReader::getColumnCount() {
  return currentRecord.size();
}

std::vector<std::string> DataProviders::XlsReader::getHeader() {
  return header;
}

uint64_t DataProviders::XlsReader::getCurrentRecordNumber() {
  return currentRecordNumber;
}

bool DataProviders::XlsReader::next() {
  if (xlsxioread_sheet_next_row(xlsxioSheet) != 0) {
    char *value;
    while ((value = xlsxioread_sheet_next_cell(xlsxioSheet)) != nullptr) {
      currentRecord.emplace_back(value);
      delete value;
    }
    currentRecordNumber++;
  } else {
    _eof = true;
    return false;
  }
  return true;
}

void DataProviders::XlsReader::first() {
  throw NotImplementedException();
}

bool DataProviders::XlsReader::eof() {
  return _eof;
}

DataProviders::XlsReader::~XlsReader() {
  xlsxioread_sheet_close(xlsxioSheet);
  xlsxioread_close(xlsxioReader);
}
