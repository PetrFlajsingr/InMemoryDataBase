//
// Created by Petr Flajsingr on 02/09/2018.
//

#ifndef CSV_READER_TESTS_ASYNCMEMORYDATASET_H
#define CSV_READER_TESTS_ASYNCMEMORYDATASET_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../AsyncMemoryDataset.h"
#include "../../Interfaces/IDataProvider.h"
#include "../../DataProviders/ArrayDataProvider.h"

class MockAsyncMemoryDatasetObserver : public IAsyncMemoryDatasetObserver {
public:
    MOCK_METHOD1(onBeforeOpen, void(AsyncMemoryDataset *sender));

    MOCK_METHOD1(onAfterOpen, void(AsyncMemoryDataset *sender));

    MOCK_METHOD1(onBeforeSort, void(AsyncMemoryDataset *sender));

    MOCK_METHOD1(onAfterSort, void(AsyncMemoryDataset *sender));

    MOCK_METHOD1(onBeforeFilter, void(AsyncMemoryDataset *sender));

    MOCK_METHOD1(onAfterFilter, void(AsyncMemoryDataset *sender));

    MOCK_METHOD1(onBeforeAppendDataProvider, void(AsyncMemoryDataset *sender));

    MOCK_METHOD1(onAfterAppendDataProvider, void(AsyncMemoryDataset *sender));
};

class AsyncMemoryDataset_tests : public ::testing::Test {
protected:
    static const unsigned int columnCount = 5;

    AsyncMemoryDataset* dataset{};

    IDataProvider* dataProvider{};

    std::string columnNamesSmall[columnCount] {
            "0", "1", "2", "3", "4"
    };
    std::vector<std::vector<std::string>> test {{"COLUMN11", "COLUMN12", "COLUMN13", "COLUMN14", "COLUMN15"},
                                                {"COLUMN21", "COLUMN22", "COLUMN23", "COLUMN24", "COLUMN25"},
                                                {"COLUMN31", "COLUMN32", "COLUMN33", "COLUMN34", "COLUMN35"},
                                                {"COLUMN41", "COLUMN42", "COLUMN43", "COLUMN44", "COLUMN45"},
                                                {"COLUMN51", "COLUMN52", "COLUMN53", "COLUMN54", "COLUMN55"}
    };

public:
    AsyncMemoryDataset_tests() = default;

    void SetUp() override {
        dataset = new AsyncMemoryDataset();
        dataProvider = new ArrayDataProvider(test);

        dataset->setDataProvider(dataProvider);

        std::vector<ValueType> types;
        for(int iter = 0; iter < dataProvider->getColumnCount(); iter++){
            types.push_back(STRING_VAL);
        }
        dataset->setFieldTypes(types);
    }

    void TearDown() override {
        delete dataset;
        delete dataProvider;
    }

    ~AsyncMemoryDataset_tests() override = default;
};

TEST_F(AsyncMemoryDataset_tests, open) {
    MockAsyncMemoryDatasetObserver observer;
    dataset->addObserver(&observer);

    EXPECT_CALL(observer, onBeforeOpen(dataset));

    EXPECT_CALL(observer, onAfterOpen(dataset));

    ASSERT_NO_THROW(dataset->open());

    dataset->removeObserver(&observer);
}

TEST_F(AsyncMemoryDataset_tests, sort) {
    FAIL();
}

TEST_F(AsyncMemoryDataset_tests, filter) {
    FAIL();
}

TEST_F(AsyncMemoryDataset_tests, append_data_provider) {
    FAIL();
}

#endif //CSV_READER_TESTS_ASYNCMEMORYDATASET_H
