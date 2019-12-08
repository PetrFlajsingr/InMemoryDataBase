//
// Created by Petr Flajsingr on 13/10/2018.
//

#include "CsvWriter.h"
#include <Exceptions.h>
#include <vector>

DataWriters::CsvWriter::CsvWriter(std::string_view filePath, std::string_view delimiter) : BaseDataWriter() {
  this->delimiter = delimiter;
  outputStream = new std::ofstream();
  outputStream->open(std::string(filePath), std::ofstream::out);
}

DataWriters::CsvWriter::CsvWriter(std::string_view filePath, CharSet charSet, std::string_view delimiter)
    : BaseDataWriter(charSet) {
  this->delimiter = delimiter;
  outputStream = new std::ofstream();
  outputStream->open(std::string(filePath), std::ofstream::out);
}

DataWriters::CsvWriter::~CsvWriter() {
  outputStream->close();
  delete outputStream;
}

void DataWriters::CsvWriter::writeHeader(const std::vector<std::string> &header) {
  if (header.empty()) {
    throw InvalidArgumentException("Headers can not be empty.");
  }
  columnCount = header.size();
  writeRow(header);
}

void DataWriters::CsvWriter::writeRecord(const std::vector<std::string> &record) {
  if (record.size() != columnCount) {
    throw InvalidArgumentException("Length of record doesn't match length of header.");
  }
  writeRow(record);
}

void replaceAll(std::string& source, const std::string& from, const std::string& to)
{
  std::string newString;
  newString.reserve(source.length()); 

  std::string::size_type lastPos = 0;
  std::string::size_type findPos;

  while(std::string::npos != (findPos = source.find(from, lastPos)))
  {
    newString.append(source, lastPos, findPos - lastPos);
    newString += to;
    lastPos = findPos + from.length();
  }

  // Care for the rest after last occurrence
  newString += source.substr(lastPos);

  source.swap(newString);
}

void DataWriters::CsvWriter::writeRow(const std::vector<std::string> &data) {
  for (gsl::index i = 0; i < static_cast<gsl::index>(columnCount - 1); ++i) {
    if (convert) {
      *outputStream << converter->convertBack(data[i]) << delimiter;
    } else if (addQuotes) {
      std::string s = data[i];
      if (!s.empty())
        replaceAll(s, "\"", "\"\"");
      *outputStream << '"' << s << '"' << delimiter;
    } else {
      *outputStream << data[i] << delimiter;
    }
  }
  if (convert) {
    *outputStream << converter->convertBack(data[columnCount - 1]) << "\n";
  } else if (addQuotes) {
    std::string s = data[columnCount - 1];
    if (!s.empty())
      replaceAll(s, "\"", "\"\"");
    *outputStream << '"' << data[columnCount - 1] << "\"\n";
  } else {
    *outputStream << data[columnCount - 1] << "\n";
  }
}

void DataWriters::CsvWriter::setAddQuotes(bool value) {
  addQuotes = value;
}
