//
// Created by Petr Flajsingr on 30/08/2018.
//

#include <gtest/gtest.h>
#include "Utilities.h"

class isDouble_isInteger_tests : public ::testing::Test {
protected:
    std::string validIntegers[12] {
        "1", "1278623", "193482", "29382",
        "+1", "+1278623", "+193482", "+29382",
        "-1", "-1278623", "-193482", "-29382"
    };

    std::string validDoubles[4] {
        "0.1334242", ".22343", "23234243.00022932", "2430.23243"
    };

    std::string invalids[6] {
            "notanumber", "NaN", "112e", "fgfDSD",
            "", "number",
    };

public:
    isDouble_isInteger_tests() = default;

    void SetUp() override {}

    void TearDown() override {}

    ~isDouble_isInteger_tests() override = default;
};

TEST_F (isDouble_isInteger_tests, isinteger) {
    for(const auto &iter : validIntegers) {
        EXPECT_TRUE(Utilities::isInteger(iter));
    }
    for(const auto &iter : validDoubles) {
        EXPECT_FALSE(Utilities::isInteger(iter));
    }
    for(const auto &iter : invalids) {
        EXPECT_FALSE(Utilities::isInteger(iter));
    }
}

TEST_F (isDouble_isInteger_tests, isdouble) {
    for(const auto &iter : validIntegers) {
        EXPECT_TRUE(Utilities::isDouble(iter));
    }
    for(const auto &iter : validDoubles) {
        EXPECT_TRUE(Utilities::isDouble(iter));
    }
    for(const auto &iter : invalids) {
        EXPECT_FALSE(Utilities::isDouble(iter));
    }
}