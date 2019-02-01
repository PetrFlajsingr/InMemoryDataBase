//
// Created by Petr Flajsingr on 2019-01-31.
//

#include <catch.hpp>
#include <Utilities.h>
#include <map>

TEST_CASE("Compare integers", "[int_compare]") {
  REQUIRE(Utilities::compareInt(0, 0) == 0);
  REQUIRE(Utilities::compareInt(1, 1) == 0);
  REQUIRE(Utilities::compareInt(100, 100) == 0);
  REQUIRE(Utilities::compareInt(-1, -1) == 0);
  REQUIRE(Utilities::compareInt(-100, -100) == 0);

  REQUIRE(Utilities::compareInt(0, 100) == -1);
  REQUIRE(Utilities::compareInt(1, 100) == -1);
  REQUIRE(Utilities::compareInt(100, 200) == -1);
  REQUIRE(Utilities::compareInt(-1, 10) == -1);
  REQUIRE(Utilities::compareInt(-100, -10) == -1);

  REQUIRE(Utilities::compareInt(100, 0) == 1);
  REQUIRE(Utilities::compareInt(100, 1) == 1);
  REQUIRE(Utilities::compareInt(200, 100) == 1);
  REQUIRE(Utilities::compareInt(10, -1) == 1);
  REQUIRE(Utilities::compareInt(-10, -100) == 1);
}

TEST_CASE("Compare doubles", "[double_compare]") {
  REQUIRE(Utilities::compareDouble(0, 0) == 0);
  REQUIRE(Utilities::compareDouble(1, 1) == 0);
  REQUIRE(Utilities::compareDouble(100, 100) == 0);
  REQUIRE(Utilities::compareDouble(-1, -1) == 0);
  REQUIRE(Utilities::compareDouble(-100, -100) == 0);

  REQUIRE(Utilities::compareDouble(0, 100) == -1);
  REQUIRE(Utilities::compareDouble(1, 100) == -1);
  REQUIRE(Utilities::compareDouble(100, 200) == -1);
  REQUIRE(Utilities::compareDouble(-1, 10) == -1);
  REQUIRE(Utilities::compareDouble(-100, -10) == -1);

  REQUIRE(Utilities::compareDouble(100, 0) == 1);
  REQUIRE(Utilities::compareDouble(100, 1) == 1);
  REQUIRE(Utilities::compareDouble(200, 100) == 1);
  REQUIRE(Utilities::compareDouble(10, -1) == 1);
  REQUIRE(Utilities::compareDouble(-10, -100) == 1);
}

TEST_CASE("Compare Currency", "[Currency_compare]") {
  std::map<double, Currency> testVals{
      {0, Currency(0)},
      {1, Currency(1)},
      {100, Currency(100)},
      {-1, Currency(-1)},
      {-100, Currency(-100)},
      {200, Currency(200)},
      {-10, Currency(-10)},
      {0.1, Currency(0.1)},
      {0.11, Currency(0.11)},
  };
  REQUIRE(Utilities::compareCurrency(testVals[0], testVals[0]) == 0);
  REQUIRE(Utilities::compareCurrency(testVals[1], testVals[1]) == 0);
  REQUIRE(Utilities::compareCurrency(testVals[100], testVals[100]) == 0);
  REQUIRE(Utilities::compareCurrency(testVals[-1], testVals[-1]) == 0);
  REQUIRE(Utilities::compareCurrency(testVals[-100], testVals[-100]) == 0);

  REQUIRE(Utilities::compareCurrency(testVals[0], testVals[100]) == -1);
  REQUIRE(Utilities::compareCurrency(testVals[1], testVals[100]) == -1);
  REQUIRE(Utilities::compareCurrency(testVals[100], testVals[200]) == -1);
  REQUIRE(Utilities::compareCurrency(testVals[-1], testVals[10]) == -1);
  REQUIRE(Utilities::compareCurrency(testVals[-100], testVals[-10]) == -1);

  REQUIRE(Utilities::compareCurrency(testVals[100], testVals[0]) == 1);
  REQUIRE(Utilities::compareCurrency(testVals[100], testVals[1]) == 1);
  REQUIRE(Utilities::compareCurrency(testVals[200], testVals[100]) == 1);
  REQUIRE(Utilities::compareCurrency(testVals[10], testVals[-1]) == 1);
  REQUIRE(Utilities::compareCurrency(testVals[-10], testVals[-100]) == 1);

  REQUIRE(Utilities::compareCurrency(testVals[0.1], testVals[0.11]) == -1);
  REQUIRE(Utilities::compareCurrency(testVals[0.11], testVals[0.1]) == 1);
}

// TODO: Compare String, Compare DateTime, CopyStringToNewChar, endsWith, getType,
// stringToDouble, stringToInt, splitStringByDelimiter/Reserve