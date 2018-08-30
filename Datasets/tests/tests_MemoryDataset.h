//
// Created by Petr Flajsingr on 30/08/2018.
//

#ifndef CSV_READER_TESTS_MEMORYDATASET_H
#define CSV_READER_TESTS_MEMORYDATASET_H

#include <gtest/gtest.h>
#include "../Memorydataset.h"
#include "../../DataProviders/ArrayDataProvider.h"

class MemoryDataset_tests : public ::testing::Test {
protected:
    Memorydataset* dataset;

    IDataProvider* dataProvider;

    std::string columnNamesSmall[5] {
            "0", "1", "2", "3", "4"
    };
    std::vector<std::vector<std::string>> test {{"COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"}, {"COLUMN1", "COLUMN2", "COLUMN3", "COLUMN4", "COLUMN5"}};

public:
    MemoryDataset_tests() = default;

    void SetUp() override {
        dataset = new Memorydataset();
        dataProvider = new ArrayDataProvider(test);
    }

    void TearDown() override {
        delete dataset;
        //delete dataProvider;
    }

    ~MemoryDataset_tests() override = default;
};

TEST_F (MemoryDataset_tests, open) {
    dataset->setDataProvider(dataProvider);

    std::vector<ValueType> types;
    for(int iter = 0; iter < dataProvider->getColumnCount(); iter++){
        types.push_back(STRING_VAL);
    }
    dataset->setFieldTypes(types);

    ASSERT_NO_THROW(dataset->open());

    auto value = dataset->getFieldNames();

    for(int i = 0; i < 5; ++i) {
        EXPECT_EQ(value[i], columnNamesSmall[i]);
    }

    dataset->close();
}

#endif //CSV_READER_TESTS_MEMORYDATASET_H
