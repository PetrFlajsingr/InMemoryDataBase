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

  readHeader();
}

const std::vector<std::string> &DataProviders::XlsReader::getRow() const {
  return currentRecord;
}

std::string DataProviders::XlsReader::getColumnName(unsigned int columnIndex) const {
  return currentRecord.at(columnIndex);
}

uint64_t DataProviders::XlsReader::getColumnCount() const {
  return currentRecord.size();
}

const std::vector<std::string> &DataProviders::XlsReader::getHeader() const {
  return header;
}

uint64_t DataProviders::XlsReader::getCurrentRecordNumber() const {
  return currentRecordNumber;
}

bool DataProviders::XlsReader::next() {
  if (xlsxioread_sheet_next_row(xlsxioSheet) != 0) {
    currentRecord.clear();
    char *value;
    while ((value = xlsxioread_sheet_next_cell(xlsxioSheet)) != nullptr) {
      currentRecord.emplace_back(value);
      delete value;
    }
    if (std::all_of(currentRecord.begin(),
                    currentRecord.end(),
                    [](const std::string &value) {
                      return value.empty();
                    })) {
      _eof = true;
      return false;
    }
    currentRecordNumber++;
  } else {
    _eof = true;
    return false;
  }
  return true;
}

void DataProviders::XlsReader::first() {
  xlsxioread_sheet_close(xlsxioSheet);
  xlsxioSheet =
      xlsxioread_sheet_open(xlsxioReader, nullptr, XLSXIOREAD_SKIP_EMPTY_ROWS);

  readHeader();
}

bool DataProviders::XlsReader::eof() const {
  return _eof;
}

DataProviders::XlsReader::~XlsReader() {
  close();
}

void DataProviders::XlsReader::close() {
  xlsxioread_sheet_close(xlsxioSheet);
  xlsxioread_close(xlsxioReader);
}

void DataProviders::XlsReader::readHeader() {
  if (xlsxioread_sheet_next_row(xlsxioSheet) != 0) {
    char *value;
    while ((value = xlsxioread_sheet_next_cell(xlsxioSheet)) != nullptr) {
      header.emplace_back(value);
      delete value;
    }
  }
}
