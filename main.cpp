#include <iostream>
#include "IDataProvider.h"
#include "CsvReader.h"

int main(int argc, char** argv) {
    IDataProvider* reader = new CsvReader(std::string(argv[1]));

    std::cout << "Col count: " << reader->getColumnCount() << std::endl;

    for(const auto &str : reader->getColumnNames()) {
        std::cout << "Col: " << str << std::endl;
    }

    for(const auto &str : reader->getRow()) {
        std::cout << "Record: " << str << std::endl;
    }

    reader->next();

    for(const auto &str : reader->getRow()) {
        std::cout << "Record: " << str << std::endl;
    }

    delete reader;

    return 0;
}