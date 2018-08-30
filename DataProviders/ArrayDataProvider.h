//
// Created by Petr Flajsingr on 30/08/2018.
//

#ifndef CSV_READER_ARRAYDATAPROVIDER_H
#define CSV_READER_ARRAYDATAPROVIDER_H


#include <utility>
#include <vector>
#include "../Interfaces/IDataProvider.h"

class ArrayDataProvider : public IDataProvider{
private:
    std::vector<std::vector<std::string>> data;

    unsigned long row = 0;


public:
    explicit ArrayDataProvider(std::vector<std::vector<std::string>> data) :
            data(std::move(data)){};

    ~ArrayDataProvider() override {}

    std::vector<std::string> getRow() override {
        std::vector<std::string> result;

        for(unsigned long i = 0; i < data[0].size(); ++i) {
            result.push_back(data[row][i]);
        }

        return result;
    }

    std::string getColumn(unsigned int columnIndex) override {
        std::to_string(columnIndex);
    }

    unsigned long getColumnCount() override {
        return data[0].size();
    }

    std::vector<std::string> getColumnNames() override {
        std::vector<std::string> result;
        for(unsigned long i = 0; i < data[0].size(); ++i) {
            result.push_back(std::to_string(i));
        }

        return result;
    }

    unsigned long getCurrentRecordNumber() override {
        return row;
    }

    bool next() override {
        if(row < data.size() - 1) {
            row++;
        }
    }

    bool previous() override {
        if(row > 0) {
            row--;
        }
    }

    void first() override {
        row = 0;
    }

    void last() override {
        row = data.size() - 1;
    }

    bool eof() override {
        return row == data.size() - 1;
    }
};



#endif //CSV_READER_ARRAYDATAPROVIDER_H
