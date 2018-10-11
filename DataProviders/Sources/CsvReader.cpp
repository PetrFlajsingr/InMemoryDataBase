//
// Created by Petr Flajsingr on 24/08/2018.
//

#include "CsvReader.h"

DataProviders::CsvReader::CsvReader(std::string filePath) {
  this->file.open(filePath);
  if (!this->file.is_open()) {
    throw IOException("File could not be opened.");
  }

  this->readHeader();
  this->parseRecord();
}

DataProviders::CsvReader::~CsvReader() {
  if (this->file.is_open()) {
    this->file.close();
  }
}

void DataProviders::CsvReader::readHeader() {
  char buffer[BUFFER_SIZE];

  this->file.getline(buffer, BUFFER_SIZE);
  header = Utilities::splitStringByDelimiter(std::string(buffer),
                                             std::string(1, DELIMITER));
}

bool DataProviders::CsvReader::next() {
  if (this->file.eof()) {
    return false;
  }

  this->parseRecord();
  this->currentRecordNumber++;

  return true;
}

void DataProviders::CsvReader::parseRecord() {
  char buffer[BUFFER_SIZE];

  this->file.getline(buffer, BUFFER_SIZE);
  this->currentRecord = Utilities::splitStringByDelimiter(std::string(buffer),
      std::string(1, DELIMITER));
}

void DataProviders::CsvReader::first() {
  this->file.clear();
  this->file.seekg(0, std::ios::beg);

  this->readHeader();
  this->parseRecord();

  this->currentRecordNumber = 0;
}

bool DataProviders::CsvReader::eof() {
  return this->file.eof();
}
