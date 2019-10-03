//
// Created by petr on 10/3/19.
//

#include "CsvStreamReader.h"

DataProviders::CsvStreamReader::CsvStreamReader(const std::string &filePath, char delimiter)
        : BaseDataProvider(), csvStream(std::make_unique<csvstream>(filePath, delimiter)), filePath(filePath),
          delimiter(delimiter) {
    header = csvStream->getheader();
}

DataProviders::CsvStreamReader::CsvStreamReader(const std::string &filePath, CharSet inputCharSet,
                                                char delimiter) : BaseDataProvider(inputCharSet), csvStream(
        std::make_unique<csvstream>(filePath, delimiter)), filePath(filePath), delimiter(delimiter) {
    header = csvStream->getheader();
}

bool DataProviders::CsvStreamReader::next() {
    if (!(*csvStream >> currentRecord)) {
        _eof = true;
        return false;
    }

    currentRecordNumber++;
    return true;
}

bool DataProviders::CsvStreamReader::eof() const {
    return _eof;
}

void DataProviders::CsvStreamReader::first() {
    currentRecordNumber = -1;
    _eof = false;
    csvStream = std::make_unique<csvstream>(filePath, delimiter);
}
