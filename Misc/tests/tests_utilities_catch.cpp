//
// Created by Petr Flajsingr on 2019-01-31.
//

#include <catch2/catch.hpp>
#include <Utilities.h>
#include <Types.h>
#include <map>

TEST_CASE("Compare integers", "[compareInt]") {
  CHECK(Utilities::compareInt(0, 0) == 0);
  CHECK(Utilities::compareInt(1, 1) == 0);
  CHECK(Utilities::compareInt(100, 100) == 0);
  CHECK(Utilities::compareInt(-1, -1) == 0);
  CHECK(Utilities::compareInt(-100, -100) == 0);

  CHECK(Utilities::compareInt(0, 100) == -1);
  CHECK(Utilities::compareInt(1, 100) == -1);
  CHECK(Utilities::compareInt(100, 200) == -1);
  CHECK(Utilities::compareInt(-1, 10) == -1);
  CHECK(Utilities::compareInt(-100, -10) == -1);

  CHECK(Utilities::compareInt(100, 0) == 1);
  CHECK(Utilities::compareInt(100, 1) == 1);
  CHECK(Utilities::compareInt(200, 100) == 1);
  CHECK(Utilities::compareInt(10, -1) == 1);
  CHECK(Utilities::compareInt(-10, -100) == 1);
}

TEST_CASE("Compare doubles", "[compareDouble]") {
  CHECK(Utilities::compareDouble(0, 0) == 0);
  CHECK(Utilities::compareDouble(1, 1) == 0);
  CHECK(Utilities::compareDouble(100, 100) == 0);
  CHECK(Utilities::compareDouble(-1, -1) == 0);
  CHECK(Utilities::compareDouble(-100, -100) == 0);

  CHECK(Utilities::compareDouble(0, 100) == -1);
  CHECK(Utilities::compareDouble(1, 100) == -1);
  CHECK(Utilities::compareDouble(100, 200) == -1);
  CHECK(Utilities::compareDouble(-1, 10) == -1);
  CHECK(Utilities::compareDouble(-100, -10) == -1);

  CHECK(Utilities::compareDouble(100, 0) == 1);
  CHECK(Utilities::compareDouble(100, 1) == 1);
  CHECK(Utilities::compareDouble(200, 100) == 1);
  CHECK(Utilities::compareDouble(10, -1) == 1);
  CHECK(Utilities::compareDouble(-10, -100) == 1);
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
  CHECK(Utilities::compareCurrency(testVals[0], testVals[0]) == 0);
  CHECK(Utilities::compareCurrency(testVals[1], testVals[1]) == 0);
  CHECK(Utilities::compareCurrency(testVals[100], testVals[100]) == 0);
  CHECK(Utilities::compareCurrency(testVals[-1], testVals[-1]) == 0);
  CHECK(Utilities::compareCurrency(testVals[-100], testVals[-100]) == 0);

  CHECK(Utilities::compareCurrency(testVals[0], testVals[100]) == -1);
  CHECK(Utilities::compareCurrency(testVals[1], testVals[100]) == -1);
  CHECK(Utilities::compareCurrency(testVals[100], testVals[200]) == -1);
  CHECK(Utilities::compareCurrency(testVals[-1], testVals[10]) == -1);
  CHECK(Utilities::compareCurrency(testVals[-100], testVals[-10]) == -1);

  CHECK(Utilities::compareCurrency(testVals[100], testVals[0]) == 1);
  CHECK(Utilities::compareCurrency(testVals[100], testVals[1]) == 1);
  CHECK(Utilities::compareCurrency(testVals[200], testVals[100]) == 1);
  CHECK(Utilities::compareCurrency(testVals[10], testVals[-1]) == 1);
  CHECK(Utilities::compareCurrency(testVals[-10], testVals[-100]) == 1);

  CHECK(Utilities::compareCurrency(testVals[0.1], testVals[0.11]) == -1);
  CHECK(Utilities::compareCurrency(testVals[0.11], testVals[0.1]) == 1);
}

TEST_CASE("Compare strings", "[compareString]") {
  CHECK(Utilities::compareString("", "") == 0);
  CHECK(Utilities::compareString("aa", "aa") == 0);
  CHECK(Utilities::compareString("abcdefg", "abcdefg") == 0);
  CHECK(Utilities::compareString("-1", "-1") == 0);

  CHECK(Utilities::compareString("abc", "bac") == -1);
  CHECK(Utilities::compareString("jujky", "ujky") == -1);

  CHECK(Utilities::compareString("bac", "abc") == 1);
  CHECK(Utilities::compareString("ujky", "jujky") == 1);
}

TEST_CASE("Compare DateTime", "[compareDateTime]") {
  DateTime dateTime1;
  dateTime1.fromString("9/10/2020 00:00:00");
  DateTime dateTime2;
  dateTime2.fromString("10/01/06 00:00:00");
  DateTime dateTime3;
  dateTime3.fromString("09/08/2020 00:00:00");
  DateTime dateTime4;
  dateTime4.fromString("12/10/2020 00:00:00");

  DateTime testVal = dateTime1;
  CHECK(Utilities::compareDateTime(dateTime1, testVal) == 0);
  testVal = dateTime2;
  CHECK(Utilities::compareDateTime(dateTime2, testVal) == 0);
  testVal = dateTime3;
  CHECK(Utilities::compareDateTime(dateTime3, testVal) == 0);
  testVal = dateTime4;
  CHECK(Utilities::compareDateTime(dateTime4, testVal) == 0);

  CHECK(Utilities::compareDateTime(dateTime1, dateTime4) == -1);
  CHECK(Utilities::compareDateTime(dateTime2, dateTime1) == -1);
  CHECK(Utilities::compareDateTime(dateTime2, dateTime3) == -1);
  CHECK(Utilities::compareDateTime(dateTime3, dateTime1) == -1);
  CHECK(Utilities::compareDateTime(dateTime3, dateTime4) == -1);

  CHECK(Utilities::compareDateTime(dateTime4, dateTime1) == 1);
  CHECK(Utilities::compareDateTime(dateTime1, dateTime2) == 1);
  CHECK(Utilities::compareDateTime(dateTime3, dateTime2) == 1);
  CHECK(Utilities::compareDateTime(dateTime1, dateTime3) == 1);
  CHECK(Utilities::compareDateTime(dateTime4, dateTime3) == 1);
}

TEST_CASE("Create zstring from string", "[copyStringToNewChar]") {

}

TEST_CASE("Check if string ends with substring", "[endsWith]") {

}

TEST_CASE("Get type of string value", "[getType]") {

}

TEST_CASE("Convert string to double", "[stringToDouble]") {

}

TEST_CASE("Split string by delimiter", "[splitStringByDelimiter]") {

}


// TODO:
//  DateTimeUtils