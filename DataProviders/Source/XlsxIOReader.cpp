//
// Created by Petr Flajsingr on 2019-01-28.
//

#include <XlsxIOReader.h>

DataProviders::XlsxIOReader::XlsxIOReader(std::string_view fileName) : BaseDataProvider() { init(fileName); }

DataProviders::XlsxIOReader::XlsxIOReader(std::string_view fileName, CharSet inputCharSet)
    : BaseDataProvider(inputCharSet) {
  init(fileName);
}

void DataProviders::XlsxIOReader::init(std::string_view fileName) {
  std::string strFileName(fileName);
  xlsxioReader = xlsxioread_open(strFileName.c_str());
  if (xlsxioReader == nullptr) {
    auto errMsg = "File could not be open: " + strFileName;
    throw IOException(errMsg.c_str());
  }
  xlsxioSheet = xlsxioread_sheet_open(xlsxioReader, nullptr, XLSXIOREAD_SKIP_EMPTY_ROWS);

  if (xlsxioSheet == nullptr) {
    auto errMsg = "Error while opening xls file: " + strFileName;
    throw IOException(errMsg.c_str());
  }

  readHeader();
}

const std::vector<std::string> &DataProviders::XlsxIOReader::getRow() const { return currentRecord; }

std::string DataProviders::XlsxIOReader::getColumnName(unsigned int columnIndex) const {
  return currentRecord.at(columnIndex);
}

uint64_t DataProviders::XlsxIOReader::getColumnCount() const { return currentRecord.size(); }

const std::vector<std::string> &DataProviders::XlsxIOReader::getHeader() const { return header; }

int DataProviders::XlsxIOReader::getCurrentRecordNumber() const { return currentRecordNumber; }

bool DataProviders::XlsxIOReader::next() {
  if (xlsxioread_sheet_next_row(xlsxioSheet) != 0) {
    currentRecord.clear();
    gsl::zstring<> value;
    while ((value = xlsxioread_sheet_next_cell(xlsxioSheet)) != nullptr) {
      if (convert) {
        currentRecord.emplace_back(converter->convert(value));
      } else {
        currentRecord.emplace_back(value);
      }
      delete value;
    }
    if (std::all_of(currentRecord.begin(), currentRecord.end(), [](std::string_view value) { return value.empty(); })) {
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

void DataProviders::XlsxIOReader::first() {
  xlsxioread_sheet_close(xlsxioSheet);
  xlsxioSheet = xlsxioread_sheet_open(xlsxioReader, nullptr, XLSXIOREAD_SKIP_EMPTY_ROWS);
  readHeader();
}

bool DataProviders::XlsxIOReader::eof() const { return _eof; }

DataProviders::XlsxIOReader::~XlsxIOReader() { close(); }

void DataProviders::XlsxIOReader::close() {
  xlsxioread_sheet_close(xlsxioSheet);
  xlsxioread_close(xlsxioReader);
}

void DataProviders::XlsxIOReader::readHeader() {
  if (xlsxioread_sheet_next_row(xlsxioSheet) != 0) {
    gsl::zstring<> value;
    while ((value = xlsxioread_sheet_next_cell(xlsxioSheet)) != nullptr) {
      if (convert) {
        header.emplace_back(converter->convert(value));
      } else {
        header.emplace_back(value);
      }
      delete value;
    }
  }
}
