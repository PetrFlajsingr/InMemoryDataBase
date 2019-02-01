//
// Created by Petr Flajsingr on 2019-01-31.
//

#include <catch.hpp>
#include <Utilities.h>
#include <DateTimeUtils.h>
#include <map>

TEST_CASE("Compare integers", "[compareInt]") {
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

TEST_CASE("Compare doubles", "[compareDouble]") {
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

TEST_CASE("Compare Currency", "[compareCurrency]") {
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

TEST_CASE("Compare strings", "[compareString]") {
  REQUIRE(Utilities::compareString("", "") == 0);
  REQUIRE(Utilities::compareString("aa", "aa") == 0);
  REQUIRE(Utilities::compareString("abcdefg", "abcdefg") == 0);
  REQUIRE(Utilities::compareString("-1", "-1") == 0);

  REQUIRE(Utilities::compareString("abc", "bac") == -1);
  REQUIRE(Utilities::compareString("jujky", "ujky") == -1);

  REQUIRE(Utilities::compareString("bac", "abc") == 1);
  REQUIRE(Utilities::compareString("ujky", "jujky") == 1);
}

TEST_CASE("Compare DateTime", "[compareDateTime]") {
  DateTime dateTime1;
  dateTime1.fromString("2020-10-09 00-00-00-0");
  DateTime dateTime2;
  dateTime2.fromString("10-01-06 00-00-00-0");
  DateTime dateTime3;
  dateTime3.fromString("2020-08-09 00-00-00-0");
  DateTime dateTime4;
  dateTime4.fromString("2020-10-12 00-00-00-0");

  DateTime testVal = dateTime1;
  REQUIRE(Utilities::compareDateTime(dateTime1, testVal) == 0);
  testVal = dateTime2;
  REQUIRE(Utilities::compareDateTime(dateTime2, testVal) == 0);
  testVal = dateTime3;
  REQUIRE(Utilities::compareDateTime(dateTime3, testVal) == 0);
  testVal = dateTime4;
  REQUIRE(Utilities::compareDateTime(dateTime4, testVal) == 0);

  REQUIRE(Utilities::compareDateTime(dateTime1, dateTime4) == -1);
  REQUIRE(Utilities::compareDateTime(dateTime2, dateTime1) == -1);
  REQUIRE(Utilities::compareDateTime(dateTime2, dateTime3) == -1);
  REQUIRE(Utilities::compareDateTime(dateTime3, dateTime1) == -1);
  REQUIRE(Utilities::compareDateTime(dateTime3, dateTime4) == -1);

  REQUIRE(Utilities::compareDateTime(dateTime4, dateTime1) == 1);
  REQUIRE(Utilities::compareDateTime(dateTime1, dateTime2) == 1);
  REQUIRE(Utilities::compareDateTime(dateTime3, dateTime2) == 1);
  REQUIRE(Utilities::compareDateTime(dateTime1, dateTime3) == 1);
  REQUIRE(Utilities::compareDateTime(dateTime4, dateTime3) == 1);
}

TEST_CASE("Create zstring from string", "[copyStringToNewChar") {

}



// TODO: CopyStringToNewChar, endsWith, getType,
//  stringToDouble, stringToInt, splitStringByDelimiter/Reserve
//  DateTimeUtils