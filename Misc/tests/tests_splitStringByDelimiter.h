//
// Created by Petr Flajsingr on 29/08/2018.
//

#include <gtest/gtest.h>
#include "../Utilities.h"
#include "../Exceptions.h"

class splitStringByDelimiter_tests : public ::testing::Test {
protected:
    static const unsigned int correctCount = 9;
    std::string inputs[correctCount] {
        "a;a",
        "a;aa",
        "a;a,bc,xy",
        "a",
        "",
        "a;;;a",
        "a\
a",
        "letadloAslonAkrava",
        "test;delimiter;end;"
    };

    std::string delimiters[correctCount] {
            ";",
            ";",
            ",",
            ";",
            ";",
            ";",
            ";",
            "A",
            ";"
    };

    std::vector<std::string> correct[correctCount] {
            {"a", "a"},
            {"a", "aa"},
            {"a;a", "bc", "xy"},
            {"a"},
            {""},
            {"a", "", "", "a"},
            {"a\
a"},
            {"letadlo", "slon", "krava"},
            {"test", "delimiter", "end", ""}
    };
public:
    splitStringByDelimiter_tests() = default;

    void SetUp() override {}

    void TearDown() override {}

    ~splitStringByDelimiter_tests() override = default;
};

TEST_F (splitStringByDelimiter_tests, correct_split) {
    for(int i = 0; i < correctCount; ++i) {
        EXPECT_EQ(Utilities::splitStringByDelimiter(inputs[i], delimiters[i]), correct[i]);
    }
}

TEST_F (splitStringByDelimiter_tests, exceptions) {
    EXPECT_THROW(Utilities::splitStringByDelimiter("a;a", ""), InvalidArgumentException);
}