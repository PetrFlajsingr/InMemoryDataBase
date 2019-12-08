//
// Created by Petr Flajsingr on 24/08/2018.
//

#include <CsvReader.h>
#include <utility>

void myGetLine(std::istream &stream, gsl::span<char> &buffer, bool allowNewLineInQuotes = true, char lineDelimiter = '\n') {
  using stream_traits = std::istream::traits_type;
  int c;
  auto streamBuffer = stream.rdbuf();
  uint32_t bufferPos = 0;

  if (allowNewLineInQuotes) {
    bool isInQuotes = false;
    while (((c = streamBuffer->sgetc()) != lineDelimiter || isInQuotes) && !stream_traits::eq_int_type(c, stream_traits::eof())) {
      if (c == '\0') {
        streamBuffer->snextc();
        continue;
      }
      if (c == '"') {
        isInQuotes = !isInQuotes;
      }
      buffer[bufferPos] = c;
      ++bufferPos;
      streamBuffer->snextc();
    }
  } else {
    while ((c = streamBuffer->sgetc()) != lineDelimiter && !stream_traits::eq_int_type(c, stream_traits::eof())) {
      if (c == '\0') {
        streamBuffer->snextc();
        continue;
      }
      buffer[bufferPos] = c;
      ++bufferPos;
      streamBuffer->snextc();
    }
  }
  if (c == lineDelimiter) {
    streamBuffer->snextc();
  }

  buffer[bufferPos] = '\0';
}

DataProviders::CsvReader::CsvReader(std::string_view filePath, std::string_view delimiter, bool useQuotes) : BaseDataProvider(), useQuotes(useQuotes) {
  init(filePath, delimiter);
}

DataProviders::CsvReader::CsvReader(std::string_view filePath, CharSet inputCharSet, std::string_view delimiter, bool useQuotes)
    : BaseDataProvider(inputCharSet), useQuotes(useQuotes) {
  init(filePath, delimiter);
}

void DataProviders::CsvReader::init(std::string_view filePath, std::string_view delim) {
  this->delimiter = delim;
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

  if (header.back().empty()) {
    header = std::vector<std::string>(header.begin(), header.end() - 1);
  }
}

bool DataProviders::CsvReader::next() {
  if (file.eof()) {
    _eof = true;
    return false;
  }

  parseRecord();
  shrinkRecordToFit();
  if (convert) {
    for (auto & i : currentRecord) {
      i = converter->convert(i);
    }
  }
  if (currentRecord.size() != header.size()) {
    _eof = true;
    return false;
  }
  currentRecordNumber++;

  return true;
}

void DataProviders::CsvReader::parseRecord() {
  char buffer[BUFFER_SIZE];
  gsl::span<char> spanBuffer{buffer, BUFFER_SIZE};

  // file.getline(buffer, BUFFER_SIZE, '\n');
  myGetLine(file, spanBuffer, false);
  auto line = std::string_view(spanBuffer.data());
  if (!line.empty() && line[line.length() - 1] == '\r') {
    line = line.substr(0, line.length() - 1);
  }
  currentRecord = tokenize(line, getColumnCount());
  for (auto &value : currentRecord) {
    value = Utilities::trim(value);
  }
}

void DataProviders::CsvReader::first() {
  file.clear();
  file.seekg(0, std::ios::beg);

  readHeader();

  _eof = false;
  currentRecordNumber = -1;
}

bool DataProviders::CsvReader::eof() const { return _eof; }

std::vector<std::string> DataProviders::CsvReader::tokenize(std::string_view line, unsigned int vectorReserve) const {
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
    case ParseState::Read: // normal read
      if (character == '\"') {
        if (useQuotes) {
          state = ParseState::QuotMark1;
          continue;
        }
      } else if (character == delimiter[0]) {
        buffer[bufferIter] = '\0';
        result.emplace_back(std::string(buffer));
        bufferIter = 0;
        break;
      }
      buffer[bufferIter] = character;
      bufferIter++;
      break;
    case ParseState::QuotMark1: // read inside " "
      if (character == '\"') {
        state = ParseState::QuotMark2;
        continue;
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
    default:
      throw IllegalStateException("Internal error. DataProviders::CsvReader::tokenize");
    }
  }
  buffer[bufferIter] = '\0';
  result.emplace_back(std::string(buffer));
  return result;
}
void DataProviders::CsvReader::shrinkRecordToFit() {
  if (currentRecord.size() > header.size()) {
    auto lastValidRecordIndex = header.size() - 1;
    for (gsl::index i = lastValidRecordIndex + 1; i < static_cast<gsl::index>(currentRecord.size()); ++i) {
      currentRecord[lastValidRecordIndex] += currentRecord[i];
    }
    currentRecord = std::vector<std::string>(currentRecord.begin(), currentRecord.begin() + lastValidRecordIndex + 1);
  }
}
