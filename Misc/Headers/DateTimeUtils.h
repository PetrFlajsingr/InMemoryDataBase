//
// Created by Petr Flajsingr on 2019-01-27.
//

#ifndef CSV_READER_DATETIMEUTILS_H
#define CSV_READER_DATETIMEUTILS_H

#include <gsl/gsl>
#include <cstdint>
#include <string>
#include <sstream>
#include <ostream>
#include <Utilities.h>

/**
 * Trida pro reprezentaci casu a data.
 */
class DateTime {
 public:
  DateTime() = default;

  explicit DateTime(std::string_view str) {
    fromString(str);
  }

  /**
   * Prevod ze string ve formatu YYYY-MM-DD HH-MM-SS
   * @param value
   */
  void fromString(std::string_view value) {
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
    Expects(0 < month && month <= 12);
    DateTime::month = month;
  }

  uint8_t getDay() const {
    return day;
  }

  void setDay(uint8_t day) {
    //  TODO
    Expects(0 < day && day <= 31);
    DateTime::day = day;
  }

  uint8_t getHour() const {
    return hour;
  }

  void setHour(uint8_t hour) {
    Expects(hour <= 24);
    DateTime::hour = hour;
  }

  uint8_t getMinute() const {
    return minute;
  }

  void setMinute(uint8_t minute) {
    Expects(minute <= 59);
    DateTime::minute = minute;
  }

  uint8_t getSecond() const {
    return second;
  }

  void setSecond(uint8_t second) {
    Expects(second <= 59);
    DateTime::second = second;
  }

  uint16_t getMillisecond() const {
    return millisecond;
  }

  void setMillisecond(uint16_t millisecond) {
    Expects(millisecond <= 999);
    DateTime::millisecond = millisecond;
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
    os << dateTime.year << "-" << dateTime.month << "-" << dateTime.day << " "
       << dateTime.hour << "-" << dateTime.minute << "-" << dateTime.second
       << " "
       << dateTime.millisecond;
    return os;
  }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
  friend std::istream &operator>>(std::istream &is, DateTime &dateTime) {
    std::string temp;

    is >> temp;
    auto dateVals = Utilities::splitStringByDelimiter(temp, "-");
    dateTime.setYear(Utilities::stringToInt(dateVals[0]));
    dateTime.setMonth(Utilities::stringToInt(dateVals[1]));
    dateTime.setDay(Utilities::stringToInt(dateVals[2]));

    is >> temp;
    auto timeVals = Utilities::splitStringByDelimiter(temp, "-");
    dateTime.setHour(Utilities::stringToInt(timeVals[0]));
    dateTime.setMinute(Utilities::stringToInt(timeVals[1]));
    dateTime.setSecond(Utilities::stringToInt(timeVals[2]));
    dateTime.setMillisecond(Utilities::stringToInt(timeVals[3]));

    return is;
  }
#pragma clang diagnostic pop

 private:
  int32_t year = 0;
  uint8_t month = 1;
  uint8_t day = 1;

  uint8_t hour = 0;
  uint8_t minute = 0;
  uint8_t second = 0;

  uint16_t millisecond = 0;

};

#endif //CSV_READER_DATETIMEUTILS_H
