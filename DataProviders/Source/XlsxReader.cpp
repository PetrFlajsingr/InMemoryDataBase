//
// Created by petr on 10/8/19.
//

#include "XlsxReader.h"
#include <utility>
#include <vector>
#include <Utilities.h>

using namespace std::string_literals;

DataProviders::XlsxReader::XlsxReader(std::string path) : BaseDataProvider(), filePath(std::move(path)) {
  open();
}

const std::vector<std::string> &DataProviders::XlsxReader::getRow() const {
  return currentRow;
}

std::string DataProviders::XlsxReader::getColumnName(unsigned int columnIndex) const {
  return header[columnIndex];
}

uint64_t DataProviders::XlsxReader::getColumnCount() const { return header.size(); }

const std::vector<std::string> &DataProviders::XlsxReader::getHeader() const {
  return header;
}

int DataProviders::XlsxReader::getCurrentRecordNumber() const {
  return currentRecordNumber;
}

bool DataProviders::XlsxReader::next() {
  if (currentRecordNumber == sheet->rows.lastcol) {
    _eof = true;
    return false;
  }
  readRow(currentRecordNumber);
  ++currentRecordNumber;
  return true;
}

void DataProviders::XlsxReader::first() {
  close();
  open();
}

bool DataProviders::XlsxReader::eof() const { throw NotImplementedException(); }

void DataProviders::XlsxReader::readHeader() {
  readRow(0);
  header = currentRow;
  currentRow.clear();
}

void DataProviders::XlsxReader::readRow(gsl::index rowNumber)  {
  xls::xlsRow *row = xls::xls_row(sheet, rowNumber);
  for (int i = 0; i < sheet->rows.lastcol; ++i) {
    xls::xlsCell *cell = &row->cells.cell[i];
    if (cell->id == 0x0201) {
      currentRow.emplace_back();
    } else if (cell->id == 0x0203) {
      currentRow.emplace_back(Utilities::doubleToString(cell->d));
    } else if (cell->id == 0x0006) {
      if (strcmp(cell->str, "bool") == 0) {
        currentRow.emplace_back(Utilities::boolToString(cell->d > 0.0));
      } else if (strcmp(cell->str, "error") == 0) {
        currentRow.emplace_back();
      } else {
        currentRow.emplace_back(cell->str);
      }
    } else if (cell->str != nullptr) {
      currentRow.emplace_back(cell->str);
    }
  }
}

void DataProviders::XlsxReader::open() {
  xls::xls_error_t error = xls::LIBXLS_OK;
  workBook = xls::xls_open_file(filePath.data(), "UTF-8", &error);
  if (workBook == nullptr) {
    throw IOException("File: "s + filePath + " could not be open.");
  }
  for (uint i = 0; i < workBook->sheets.count; i++) { // sheets
    auto whatever = workBook->sheets.sheet[i];
    std::cout << whatever.name << std::endl;
  }
  sheet = xls::xls_getWorkSheet(workBook, 1);
  readHeader();
}

void DataProviders::XlsxReader::close() {
  xls::xls_close_WS(sheet);
  xls::xls_close_WB(workBook);
}

DataProviders::XlsxReader::~XlsxReader() {
  close();
}
