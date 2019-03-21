//
// Created by Petr Flajsingr on 24/08/2018.
//

#include <utility>
#include <CsvReader.h>

DataProviders::CsvReader::CsvReader(std::string_view filePath, std::string_view delimiter)
    : BaseDataProvider() {
  init(filePath, delimiter);
}

DataProviders::CsvReader::CsvReader(std::string_view filePath,
                                    CharSet inputCharSet,
                                    std::string_view delimiter)
    : BaseDataProvider(inputCharSet) {
  init(filePath, delimiter);
}

void DataProviders::CsvReader::init(std::string_view filePath, std::string_view delimiter) {
  this->delimiter = delimiter;
  file.open(std::string(filePath));
  if (!file.is_open()) {
    std::string errMsg = "File could not be open: " + std::string(filePath);
    throw IOException(errMsg.c_str());
  }
  readHeader();
}

DataProviders::CsvReader::~CsvReader() {
  if (file.is_open()) {
    file.close();
  }
}

void DataProviders::CsvReader::readHeader() {
  char buffer[BUFFER_SIZE];

  file.getline(buffer, BUFFER_SIZE);
  auto line = std::string_view(buffer);
  while (!isalnum(line[0]) && line[0] != '\"') {
    line = line.substr(1);
  }
  if (!line.empty() && line[line.length() - 1] == '\r') {
    line = line.substr(0, line.length() - 1);
  }
  header = tokenize(line, 1);
}

bool DataProviders::CsvReader::next() {
  if (file.eof()) {
    _eof = true;
    return false;
  }

  parseRecord();
  if (currentRecord.size() != header.size()) {
    return false;
  }
  currentRecordNumber++;

  return true;
}

void DataProviders::CsvReader::parseRecord() {
  char buffer[BUFFER_SIZE];

  file.getline(buffer, BUFFER_SIZE);
  auto line = std::string_view(buffer);
  if (!line.empty() && line[line.length() - 1] == '\r') {
    line = line.substr(0, line.length() - 1);
  }
  currentRecord = tokenize(line, getColumnCount());
}

void DataProviders::CsvReader::first() {
  file.clear();
  file.seekg(0, std::ios::beg);

  readHeader();

  _eof = false;
  currentRecordNumber = -1;
}

bool DataProviders::CsvReader::eof() const {
  return _eof;
}

std::vector<std::string> DataProviders::CsvReader::tokenize(std::string_view line,
                                                            unsigned int vectorReserve) const {
  std::string innerLine = std::string(line);
  if (convert) {
    innerLine = converter->convert(innerLine);
  }
  char buffer[BUFFER_SIZE];
  uint64_t bufferIter = 0;
  ParseState state = ParseState::Read;
  /*
   * Popis stavu:
   *  - Read: Bezne cteni bunky, preruseno pri nalezeni delimiteru
   *  - QuotMark1: Uvnitr uvozovek, vynechava delimiter
   *  - QuatMark2: Pri nalezeni druhe uvozovky, kontroluje, jestli neni v zaznamu '""' reprezentujici uvozovku
   */

  std::vector<std::string> result;
  result.reserve(vectorReserve);
  for (auto character : innerLine) {
    switch (state) {
      case ParseState::Read:  // normal read
        if (character == '\"') {
          state = ParseState::QuotMark1;
        } else if (character == delimiter[0]) {
          buffer[bufferIter] = '\0';
          result.emplace_back(std::string(buffer));
          bufferIter = 0;
          break;
        }
        buffer[bufferIter] = character;
        bufferIter++;
        break;
      case ParseState::QuotMark1:  // read inside " "
        if (character == '\"') {
          state = ParseState::QuotMark2;
        }
        buffer[bufferIter] = character;
        bufferIter++;
        break;
      case ParseState::QuotMark2:
        if (character == '\"') {
          state = ParseState::QuotMark1;
        } else if (character == delimiter[0]) {
          buffer[bufferIter] = '\0';
          result.emplace_back(std::string(buffer));
          bufferIter = 0;
          state = ParseState::Read;
          break;
        } else {
          state = ParseState::Read;
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
