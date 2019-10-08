//
// Created by petr on 10/8/19.
//

#include "XlsReader.h"
const std::vector<std::string> &DataProviders::XlsReader::getRow() const { throw NotImplementedException(); }

std::string DataProviders::XlsReader::getColumnName(unsigned int columnIndex) const { throw NotImplementedException(); }

uint64_t DataProviders::XlsReader::getColumnCount() const { throw NotImplementedException(); }

const std::vector<std::string> &DataProviders::XlsReader::getHeader() const { throw NotImplementedException(); }

int DataProviders::XlsReader::getCurrentRecordNumber() const { throw NotImplementedException(); }

bool DataProviders::XlsReader::next() { throw NotImplementedException(); }

void DataProviders::XlsReader::first() {throw NotImplementedException();}

bool DataProviders::XlsReader::eof() const { throw NotImplementedException(); }
