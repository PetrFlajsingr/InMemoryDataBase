//
// Created by Petr Flajsingr on 2019-01-27.
//

#ifndef CSV_READER_DATETIMEUTILS_H
#define CSV_READER_DATETIMEUTILS_H

#include <cstdint>
#include <string>
#include <sstream>
#include <ostream>
#include "Utilities.h"

enum DateTimeMode { DateTime_s, Date_s, Time_s };

/**
 * Trida pro reprezentaci casu a data.
 */
class DateTime {
 private:
  int32_t year = 0;
  uint8_t month = 1;
  uint8_t day = 1;

  uint8_t hour = 0;
  uint8_t minute = 0;
  uint8_t second = 0;

  uint16_t millisecond = 0;

  DateTimeMode mode;

 public:

  /**
   *
   * @param mode Urcuje typ ukladane hodnoty
   */
  explicit DateTime(DateTimeMode mode) : mode(mode) {}

  /**
   * Prevod ze string ve formatu YYYY-MM-DD HH-MM-SS
   * @param value
   */
  void fromString(const std::string &value) {
    std::stringstream ss;
    ss << value;
    ss >> *this;
  }

  /**
   * Prevod do string ve formatu YYYY-MM-DD HH-MM-SS
   * @return Casova hodnota ve formatu YYYY-MM-DD HH-MM-SS
   */
  std::string toString() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
  }

  int32_t getYear() const {
    return year;
  }

  void setYear(int32_t year) {
    DateTime::year = year;
  }

  uint8_t getMonth() const {
    return month;
  }

  void setMonth(uint8_t month) {
    DateTime::month = month;
  }

  uint8_t getDay() const {
    return day;
  }

  void setDay(uint8_t day) {
    DateTime::day = day;
  }

  uint8_t getHour() const {
    return hour;
  }

  void setHour(uint8_t hour) {
    DateTime::hour = hour;
  }

  uint8_t getMinute() const {
    return minute;
  }

  void setMinute(uint8_t minute) {
    DateTime::minute = minute;
  }

  uint8_t getSecond() const {
    return second;
  }

  void setSecond(uint8_t second) {
    DateTime::second = second;
  }

  uint16_t getMillisecond() const {
    return millisecond;
  }

  void setMillisecond(uint16_t millisecond) {
    DateTime::millisecond = millisecond;
  }

  DateTimeMode getMode() const {
    return mode;
  }

  // operator overloads
  bool operator==(const DateTime &rhs) const {
    return year == rhs.year &&
        month == rhs.month &&
        day == rhs.day &&
        hour == rhs.hour &&
        minute == rhs.minute &&
        second == rhs.second &&
        millisecond == rhs.millisecond;
  }

  bool operator!=(const DateTime &rhs) const {
    return !(rhs == *this);
  }

  bool operator<(const DateTime &rhs) const {
    if (year < rhs.year)
      return true;
    if (rhs.year < year)
      return false;
    if (month < rhs.month)
      return true;
    if (rhs.month < month)
      return false;
    if (day < rhs.day)
      return true;
    if (rhs.day < day)
      return false;
    if (hour < rhs.hour)
      return true;
    if (rhs.hour < hour)
      return false;
    if (minute < rhs.minute)
      return true;
    if (rhs.minute < minute)
      return false;
    if (second < rhs.second)
      return true;
    if (rhs.second < second)
      return false;
    return millisecond < rhs.millisecond;
  }

  bool operator>(const DateTime &rhs) const {
    return rhs < *this;
  }

  bool operator<=(const DateTime &rhs) const {
    return !(rhs < *this);
  }

  bool operator>=(const DateTime &rhs) const {
    return !(*this < rhs);
  }

  friend std::ostream &operator<<(std::ostream &os, const DateTime &dateTime) {
    switch (dateTime.mode) {
      case DateTime_s:
        os << dateTime.year << "-" << dateTime.month << "-" << dateTime.day << " "
            << dateTime.hour << "-" << dateTime.minute << "-" << dateTime.second << " "
            << dateTime.millisecond;
        break;
      case Date_s:os << dateTime.year << "-" << dateTime.month << "-" << dateTime.day;
        break;
      case Time_s:
        os << dateTime.hour << "-" << dateTime.minute << "-" << dateTime.second << "-"
            << dateTime.millisecond;
        break;
    }

    return os;
  }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
  friend std::istream &operator>>(std::istream &is, DateTime &dateTime) {
    std::string temp;

    if (dateTime.mode == DateTime_s || dateTime.mode == Date_s) {
      is >> temp;
      auto dateVals = Utilities::splitStringByDelimiter(temp, "-");
      dateTime.year = Utilities::stringToInt(dateVals[0]);
      dateTime.month = Utilities::stringToInt(dateVals[1]);
      dateTime.day = Utilities::stringToInt(dateVals[2]);
    }

    if (dateTime.mode == DateTime_s || dateTime.mode == Time_s) {
      is >> temp;
      auto timeVals = Utilities::splitStringByDelimiter(temp, "-");
      dateTime.hour = Utilities::stringToInt(timeVals[0]);
      dateTime.minute = Utilities::stringToInt(timeVals[1]);
      dateTime.second = Utilities::stringToInt(timeVals[2]);
      dateTime.millisecond = Utilities::stringToInt(timeVals[3]);
    }

    return is;
  }
#pragma clang diagnostic pop
};

#endif //CSV_READER_DATETIMEUTILS_H
