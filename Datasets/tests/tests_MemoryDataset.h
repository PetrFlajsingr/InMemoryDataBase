//
// Created by Petr Flajsingr on 30/08/2018.
//

#ifndef CSV_READER_TESTS_MEMORYDATASET_H
#define CSV_READER_TESTS_MEMORYDATASET_H

#include <gtest/gtest.h>
#include <vector>
#include "../Memorydataset.h"
#include "../../DataProviders/ArrayDataProvider.h"
#include "../../Interfaces/IField.h"
#include "../../Misc/Exceptions.h"

class MemoryDataset_tests : public ::testing::Test {
protected:
    static const unsigned int columnCount = 5;

    Memorydataset* dataset{};

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
    MemoryDataset_tests() = default;

    void SetUp() override {
        dataset = new Memorydataset();
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
        //delete dataProvider;
    }

    ~MemoryDataset_tests() override = default;
};

TEST_F (MemoryDataset_tests, open) {
    ASSERT_NO_THROW(dataset->open());

    auto value = dataset->getFieldNames();

    for(int i = 0; i < 5; ++i) {
        EXPECT_EQ(value[i], columnNamesSmall[i]);
    }

    dataset->close();
}

TEST_F (MemoryDataset_tests, columns) {
    ASSERT_NO_THROW(dataset->open());

    auto values = dataset->getFieldNames();

    ASSERT_EQ(5, values.size());
    for(int i = 0; i < values.size(); ++i) {
        EXPECT_EQ(values[i], columnNamesSmall[i]);
    }
}

TEST_F (MemoryDataset_tests, fields) {
    std::vector<IField*> fieldVector{};

    unsigned int i = 0;
    for(auto iter : columnNamesSmall) {
        fieldVector.push_back(dataset->fieldByIndex(i));

        EXPECT_EQ(dataset->fieldByName(iter), fieldVector[i]);
        EXPECT_EQ(fieldVector[i]->getFieldName(), iter);
        i++;
    }


}

TEST_F (MemoryDataset_tests, read) {
    ASSERT_NO_THROW(dataset->open());

    unsigned int row = 0;
    while(!dataset->eof()) {
        for(int j = 0; j < 5; ++j) {
            EXPECT_EQ(dataset->fieldByIndex(j)->getAsString(), test[row][j]);
        }
        row++;
        dataset->next();
    }

    EXPECT_EQ(row, 5);
}

TEST_F (MemoryDataset_tests, move_in_dataset) {
    ASSERT_NO_THROW(dataset->open());

    std::vector<IField*> fields;
    for(int i = 0; i < dataset->getFieldNames().size(); ++i) {
        fields.push_back(dataset->fieldByIndex(i));
    }

    std::vector<std::string> temp;
    for(auto field : fields) {
        temp.push_back(field->getAsString());
    }
    //
    dataset->next();

    int i = 0;
    for(auto field : fields) {
        EXPECT_NE(field->getAsString(), temp[i]);
        i++;
    }
    //
    dataset->previous();

    i = 0;
    for(auto field : fields) {
        EXPECT_EQ(field->getAsString(), temp[i]);
        i++;
    }
    //
    dataset->first();

    i = 0;
    for(auto field : fields) {
        EXPECT_EQ(field->getAsString(), temp[i]);
        i++;
    }
    //
    dataset->last();

    i = 0;
    temp.clear();
    for(auto field : fields) {
        temp.push_back(field->getAsString());
        i++;
    }
    //
    dataset->first();
    //
    while(!dataset->eof()) {
        dataset->next();
    }

    i = 0;
    for(auto field : fields) {
        EXPECT_EQ(field->getAsString(), temp[i]);
        i++;
    }
}

TEST_F (MemoryDataset_tests, filter_equals) {
    ASSERT_NO_THROW(dataset->open());

    FilterOptions options;
    for(int i = 0; i < test[0].size(); ++i) {
        options.addOption(i, "COLUMN" + std::to_string(i), EQUALS);
        dataset->filter(options);

        EXPECT_TRUE(dataset->eof());

        options.options.clear();
    }
    for(int i = 0; i < test.size(); ++i) {
        for (int j = 0; j < test[i].size(); ++j) {
            options.addOption(j, "COLUMN" + std::to_string(i + 1) + std::to_string(j + 1), EQUALS);
            dataset->filter(options);

            ASSERT_FALSE(dataset->eof());
            dataset->first();
            EXPECT_EQ(dataset->fieldByIndex(j)->getAsString(), test[i][j]);

            options.options.clear();
        }
    }
}

TEST_F (MemoryDataset_tests, filter_starts_with) {
    ASSERT_NO_THROW(dataset->open());

    FilterOptions options;
    for(int i = 0; i < test[0].size(); ++i) {
        options.addOption(i, "COL", STARTS_WITH);
        dataset->filter(options);

        ASSERT_FALSE(dataset->eof());

        options.options.clear();

        dataset->first();

        EXPECT_EQ(dataset->fieldByIndex(i)->getAsString(), test[0][i]);
    }
}

TEST_F (MemoryDataset_tests, filter_ends_with) {
    ASSERT_NO_THROW(dataset->open());

    FilterOptions options;
    for(int i = 0; i < test[0].size(); ++i) {
        options.addOption(i, "1", ENDS_WITH);
        dataset->filter(options);

        options.options.clear();

        dataset->first();
        if(i == 0) {
            ASSERT_FALSE(dataset->eof());
            for(int j = 0; j < test.size(); ++j) {
                EXPECT_EQ(dataset->fieldByIndex(i)->getAsString(), test[j][i]);
                dataset->next();
            }
        } else {
            ASSERT_TRUE(dataset->eof());
        }
    }
}

TEST_F (MemoryDataset_tests, sort) {
    ASSERT_NO_THROW(dataset->open());

    //
    dataset->sort(0, DESCENDING);
    IField* col1 = dataset->fieldByIndex(0);

    std::string previous = col1->getAsString();

    dataset->next();
    while(!dataset->eof()) {
        EXPECT_TRUE(std::strcmp(col1->getAsString().c_str(), previous.c_str()) < 0);

        previous = col1->getAsString();
        dataset->next();
    }
    //\
    //
    dataset->sort(3, ASCENDING);
    IField* col3 = dataset->fieldByIndex(3);

    previous = col3->getAsString();

    dataset->next();
    while(!dataset->eof()) {
        EXPECT_TRUE(std::strcmp(col3->getAsString().c_str(), previous.c_str()) > 0);

        previous = col3->getAsString();
        dataset->next();
    }
}

TEST_F (MemoryDataset_tests, append) {
    ASSERT_NO_THROW(dataset->open());

    std::vector<std::string> last;

    dataset->last();
    for(int i = 0; i < test[0].size(); ++i) {
        last.push_back(dataset->fieldByIndex(i)->getAsString());
    }

    dataset->append();
    for(unsigned long i = 0; i < test[0].size(); ++i) {
        dataset->fieldByIndex(i)->setAsString(std::to_string(i));
    }

    for(int i = 0; i < test[0].size(); ++i) {
        EXPECT_EQ(dataset->fieldByIndex(i)->getAsString(), std::to_string(i));
    }

    dataset->previous();
    for(int i = 0; i < test[0].size(); ++i) {
        EXPECT_EQ(dataset->fieldByIndex(i)->getAsString(), last[i]);
    }

    dataset->next();
    for(int i = 0; i < test[0].size(); ++i) {
        EXPECT_EQ(dataset->fieldByIndex(i)->getAsString(), std::to_string(i));
    }
}

TEST_F (MemoryDataset_tests, exceptions) {
    EXPECT_THROW(dataset->sort(233, ASCENDING), InvalidArgumentException);

    EXPECT_THROW(dataset->fieldByName("this column can't exist"), InvalidArgumentException);
}

#endif //CSV_READER_TESTS_MEMORYDATASET_H
