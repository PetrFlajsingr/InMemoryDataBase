//
// Created by petr on 10/3/19.
//

#ifndef CSV_READER_CSVSTREAMREADER_H
#define CSV_READER_CSVSTREAMREADER_H

#include "BaseDataProvider.h"
#include <csvstream.h>

namespace DataProviders {
class CsvStreamReader : public BaseDataProvider {
public:
    /**
     * If the file can not be opened throws IOException.
     * @param filePath path to file
     * @param delimiter csv delimiter
     */
    explicit CsvStreamReader(std::string filePath, char delimiter = ',');

    explicit CsvStreamReader(std::string filePath, CharSet inputCharSet, char delimiter = ',');

    inline const std::vector<std::string> &getRow() const override {
        return currentRecord;
    }

    inline std::string getColumnName(unsigned int columnIndex) const override {
        return header.at(columnIndex);
    }

    bool next() override;

    void first() override;

    int getCurrentRecordNumber() const override {
        return currentRecordNumber;
    }

    inline const std::vector<std::string> &getHeader() const override {
        return header;
    }

    inline uint64_t getColumnCount() const override {
        return header.size();
    }

    bool eof() const override;

private:
    int currentRecordNumber = -1;

    std::vector<std::string> header;

    std::vector<std::string> currentRecord;

    std::unique_ptr<csvstream> csvStream;

    std::string filePath;
    char delimiter;
    bool _eof = false;

};
}  // namespace DataProviders


#endif //CSV_READER_CSVSTREAMREADER_H
