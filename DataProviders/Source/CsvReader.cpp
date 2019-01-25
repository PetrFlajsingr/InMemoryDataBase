//
// Created by Petr Flajsingr on 24/08/2018.
//

#include <iostream>
#include <CsvReader.h>

DataProviders::CsvReader::CsvReader(std::string filePath, std::string delimiter) {
  this->delimiter = delimiter;
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
  auto line = std::string(buffer);
  if (!line.empty() && line[line.length() - 1] == '\r') {
    line = line.substr(0, line.length() - 1);
  }
  header = tokenize(line, 1);
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
  auto line = std::string(buffer);
  if (!line.empty() && line[line.length() - 1] == '\r') {
    line = line.substr(0, line.length() - 1);
  }
  currentRecord = tokenize(line,
                           getColumnCount());
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

void DataProviders::CsvReader::setDelimiter(char delimiter) {
  this->delimiter = delimiter;
}

std::vector<std::string> DataProviders::CsvReader::tokenize(std::string &line, int vectorReserve) {
  char buffer[BUFFER_SIZE];
  uint64_t bufferIter = 0;
  TokeniserStates state = Read; //0 - normal read, 1 - read inside "", 2 - found " inside ""

  std::vector<std::string> result;
  result.reserve(vectorReserve);
  for (auto character : line) {
    switch (state) {
      case Read:  // normal read
        if (character == '\"') {
          state = QuotMark1;
        } else if (character == delimiter[0]) {
          buffer[bufferIter] = '\0';
          result.emplace_back(std::string(buffer));
          bufferIter = 0;
          break;
        }
        buffer[bufferIter] = character;
        bufferIter++;
        break;
      case QuotMark1:  // read inside " "
        if (character == '\"') {
          state = QuotMark2;
        }
        buffer[bufferIter] = character;
        bufferIter++;
        break;
      case QuotMark2:
        if (character == '\"') {
          state = QuotMark1;
        } else if (character == delimiter[0]) {
          buffer[bufferIter] = '\0';
          result.emplace_back(std::string(buffer));
          bufferIter = 0;
          state = Read;
          break;
        } else {
          state = Read;
        }
        buffer[bufferIter] = character;
        bufferIter++;
        break;
      default:throw IllegalStateException("Internal error. DataProviders::CsvReader::tokenize");
    }
  }
  buffer[bufferIter] = '\0';
  result.emplace_back(std::string(buffer));
  return result;
}
