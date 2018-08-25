//
// Created by Petr Flajsingr on 24/08/2018.
//

#include "CsvReader.h"
#include "Exceptions.h"
#include "Utilities.h"


CsvReader::CsvReader(std::string filePath) {
    this->file.open(filePath);

    if(!this->file.is_open()) {
        throw IOException("File could not be opened.");
    }

    this->readHeader();

    this->parseRecord();
}

CsvReader::~CsvReader() {
    if(this->file.is_open()) {
        this->file.close();
    }
}


void CsvReader::readHeader() {
    char buffer[BUFFER_SIZE];

    this->file.getline(buffer, BUFFER_SIZE);

    header = Utilities::SplitStringByDelimiter(std::string(buffer), std::string(1, DELIMITER));
}


bool CsvReader::next() {
    if(this->file.eof()) {
        return false;
    }

    this->parseRecord();

    this->currentRecordNumber++;

    return true;
}

void CsvReader::parseRecord() {
    char buffer[BUFFER_SIZE];

    this->file.getline(buffer, BUFFER_SIZE);

    this->currentRecord = Utilities::SplitStringByDelimiter(std::string(buffer), std::string(1, DELIMITER));
}

void CsvReader::first() {
    this->file.clear();
    this->file.seekg(0, std::ios::beg);

    this->parseRecord();

    this->currentRecordNumber = 0;
}
