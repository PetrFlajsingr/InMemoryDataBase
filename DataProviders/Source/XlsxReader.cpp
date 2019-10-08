//
// Created by petr on 10/8/19.
//

#include "XlsxReader.h"
#include <vector>

const std::vector<std::string> &DataProviders::XlsxReader::getRow() const { throw NotImplementedException(); }

std::string DataProviders::XlsxReader::getColumnName(unsigned int columnIndex) const { throw NotImplementedException(); }

uint64_t DataProviders::XlsxReader::getColumnCount() const { throw NotImplementedException(); }

const std::vector<std::string> &DataProviders::XlsxReader::getHeader() const { throw NotImplementedException(); }

int DataProviders::XlsxReader::getCurrentRecordNumber() const { throw NotImplementedException(); }

bool DataProviders::XlsxReader::next() { throw NotImplementedException(); }

void DataProviders::XlsxReader::first() {throw NotImplementedException();}

bool DataProviders::XlsxReader::eof() const { throw NotImplementedException(); }
