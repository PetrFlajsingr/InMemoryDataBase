//
// Created by Petr Flajsingr on 24/08/2018.
//

#include "CsvReader.h"

DataProviders::CsvReader::CsvReader(std::string filePath) {
  file.open(filePath);
  if (!file.is_open()) {
    throw IOException("File could not be open.");
  }

  readHeader();
  parseRecord();
}

DataProviders::CsvReader::~CsvReader() {
  if (file.is_open()) {
    file.close();
  }
}

void DataProviders::CsvReader::readHeader() {
  char buffer[BUFFER_SIZE];

  file.getline(buffer, BUFFER_SIZE);
  header = Utilities::splitStringByDelimiter(std::string(buffer),
                                             std::string(1, DELIMITER));
}

bool DataProviders::CsvReader::next() {
  if (file.eof()) {
    return false;
  }

  parseRecord();
  currentRecordNumber++;

  return true;
}

void DataProviders::CsvReader::parseRecord() {
  char buffer[BUFFER_SIZE];

  file.getline(buffer, BUFFER_SIZE);
  currentRecord = Utilities::splitStringByDelimiter(std::string(buffer),
      std::string(1, DELIMITER));
}

void DataProviders::CsvReader::first() {
  file.clear();
  file.seekg(0, std::ios::beg);

  readHeader();
  parseRecord();

  currentRecordNumber = 0;
}

bool DataProviders::CsvReader::eof() {
  return file.eof();
}
