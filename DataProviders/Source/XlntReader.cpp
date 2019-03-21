//
// Created by Petr Flajsingr on 2019-03-19.
//

#include <Exceptions.h>
#include <XlntReader.h>

DataProviders::XlntReader::XlntReader(std::string_view fileName, std::string_view sheetName)
    : BaseDataProvider(), fileName(fileName), sheetName(sheetName),
      lastUnusedCell((prepareSheet(), wb.read_cell())) {
  readHeader();
}

DataProviders::XlntReader::XlntReader(std::string_view fileName,
                                      CharSet inputCharSet,
                                      std::string_view sheetName)
    : BaseDataProvider(inputCharSet), fileName(fileName), sheetName(sheetName),
      lastUnusedCell((prepareSheet(), wb.read_cell())) {
  readHeader();
}

const std::vector<std::string> &DataProviders::XlntReader::getRow() const {
  return currentRecord;
}
std::string DataProviders::XlntReader::getColumnName(unsigned int columnIndex) const {
  return header.at(columnIndex);
}
uint64_t DataProviders::XlntReader::getColumnCount() const {
  return header.size();
}
const std::vector<std::string> &DataProviders::XlntReader::getHeader() const {
  return header;
}
int DataProviders::XlntReader::getCurrentRecordNumber() const {
  return currentRecordIndex;
}
bool DataProviders::XlntReader::next() {
  readRecord();
  return !eof();
}
void DataProviders::XlntReader::first() {
  currentRecordIndex = 0;
  wb.close();
  wb.open(fileName);
  lastUnusedCell = wb.read_cell();
  readHeader();
  _eof = false;
}
bool DataProviders::XlntReader::eof() const {
  return _eof;
}

void DataProviders::XlntReader::readHeader() {
  header.clear();
  bool readFirst = false;
  if (convert) {
    header.emplace_back(converter->convert(lastUnusedCell.value<std::string>()));
  } else {
    header.emplace_back(lastUnusedCell.value<std::string>());
  }
  while (wb.has_cell()) {
    auto cell = wb.read_cell();
    if (readFirst && cell.column_index() == 1) {
      lastUnusedCell = cell;
      break;
    }
    if (convert) {
      header.emplace_back(converter->convert(cell.value<std::string>()));
    } else {
      header.emplace_back(cell.value<std::string>());
    }
    readFirst = true;
  }
}
void DataProviders::XlntReader::readRecord() {
  ++currentRecordIndex;
  currentRecord.clear();
  bool readFirst = false;
  if (convert) {
    currentRecord.emplace_back(converter->convert(lastUnusedCell.value<std::string>()));
  } else {
    currentRecord.emplace_back(lastUnusedCell.value<std::string>());
  }
  while (wb.has_cell()) {
    auto cell = wb.read_cell();
    if (readFirst && cell.column_index() == 1) {
      lastUnusedCell = cell;
      break;
    }
    if (convert) {
      currentRecord.emplace_back(converter->convert(cell.value<std::string>()));
    } else {
      currentRecord.emplace_back(cell.value<std::string>());
    }
    readFirst = true;
  }
  if (!wb.has_cell()) {
    _eof = true;
  }
}
void DataProviders::XlntReader::prepareSheet() {
  wb.open(std::string(fileName));
  if (sheetName.empty()) {
    wb.begin_worksheet(wb.sheet_titles()[0]);
  } else {
    wb.begin_worksheet(sheetName);
  }
}
