//
// Created by Petr Flajsingr on 29/08/2018.
//

#include <gtest/gtest.h>
#include "Utilities.h"
#include "Exceptions.h"

TEST (Utilities, SplitStringByDelimiter) {
    EXPECT_EQ(Utilities::SplitStringByDelimiter("a;a", ";"), std::vector<std::string>({"a", "a"}));
    EXPECT_NE(Utilities::SplitStringByDelimiter("a;aa", ";"), std::vector<std::string>({"a", "a"}));
    EXPECT_EQ(Utilities::SplitStringByDelimiter("a;a,bc,xy", ","), std::vector<std::string>({"a;a", "bc", "xy"}));
    EXPECT_EQ(Utilities::SplitStringByDelimiter("a", ";"), std::vector<std::string>({"a"}));
    EXPECT_THROW(Utilities::SplitStringByDelimiter("a", ""), InvalidArgumentException);
    EXPECT_EQ(Utilities::SplitStringByDelimiter("", ";"), std::vector<std::string>({""}));
    EXPECT_EQ(Utilities::SplitStringByDelimiter("a;;;a", ";"), std::vector<std::string>({"a", "", "", "a"}));
    EXPECT_EQ(Utilities::SplitStringByDelimiter("a\
a", ";"), std::vector<std::string>({"a\
a"}));
}

TEST (Utilities, isInteger) {
    EXPECT_TRUE(Utilities::isInteger("1"));
    EXPECT_TRUE(Utilities::isInteger("+10"));
    EXPECT_TRUE(Utilities::isInteger("13274912842"));
    EXPECT_FALSE(Utilities::isInteger("1.0"));
    EXPECT_TRUE(Utilities::isInteger("-1"));
    EXPECT_FALSE(Utilities::isInteger("+-1"));
}

TEST (Utilities, isDouble) {
    EXPECT_TRUE(Utilities::isDouble("10"));
    EXPECT_TRUE(Utilities::isDouble("+10"));
    EXPECT_TRUE(Utilities::isDouble("-10"));
    EXPECT_TRUE(Utilities::isDouble("10.0123"));
    EXPECT_TRUE(Utilities::isDouble("-10.10"));
    EXPECT_FALSE(Utilities::isDouble("10."));
    EXPECT_TRUE(Utilities::isDouble(".93483483"));
}

TEST (Utilities, StringToDouble) {
    EXPECT_FLOAT_EQ(Utilities::StringToDouble("1.124242"), 1.124242);
}