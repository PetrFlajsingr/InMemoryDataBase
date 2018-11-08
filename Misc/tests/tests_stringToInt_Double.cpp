//
// Created by Petr Flajsingr on 30/08/2018.
//

#include <gtest/gtest.h>

class stringToInt_Double_tests : public ::testing::Test {
 protected:
  static const unsigned int intCount = 12;
  int integers[intCount]{
      1, 1278623, 193482, 29382,
      +1, +1278623, +193482, +29382,
      -1, -1278623, -193482, -29382
  };

  std::string integerStrings[intCount]{
      "1", "1278623", "193482", "29382",
      "+1", "+1278623", "+193482", "+29382",
      "-1", "-1278623", "-193482", "-29382"
  };

  static const unsigned int doubleCount = 4;

  double doubles[4]{
      0.1334242, .22343, 23234243.00022932, 2430.23243
  };

  std::string doubleStrings[4]{
      "0.1334242", ".22343", "23234243.00022932", "2430.23243"
  };

 public:
  stringToInt_Double_tests() = default;

  void SetUp() override {}

  void TearDown() override {}

  ~stringToInt_Double_tests() override = default;
};

TEST_F (stringToInt_Double_tests, stringToInt_test) {
  for (auto i = 0; i < intCount; ++i) {
    EXPECT_EQ(Utilities::stringToInt(integerStrings[i]), integers[i]);
  }
}

TEST_F (stringToInt_Double_tests, stringToDouble_test) {
  for (auto i = 0; i < intCount; ++i) {
    EXPECT_EQ(Utilities::stringToInt(integerStrings[i]), integers[i]);
  }
  for (auto i = 0; i < doubleCount; ++i) {
    EXPECT_EQ(Utilities::stringToDouble(doubleStrings[i]), doubles[i]);
  }
}

