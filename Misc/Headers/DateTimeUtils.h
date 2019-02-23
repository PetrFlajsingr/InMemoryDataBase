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

/**
 * Trida pro reprezentaci casu a data.
 */
class DateTime {
 public:
  DateTime() = default;

  explicit DateTime(std::string_view str);

  /**
   * Prevod ze string ve formatu YYYY-MM-DD HH-MM-SS
   * @param value
   */
  void fromString(std::string_view value);

  /**
   * Prevod do string ve formatu YYYY-MM-DD HH-MM-SS
   * @return Casova hodnota ve formatu YYYY-MM-DD HH-MM-SS
   */
  std::string toString() const;

  int32_t getYear() const;

  void setYear(int32_t year);

  uint8_t getMonth() const;

  void setMonth(uint8_t month);

  uint8_t getDay() const;

  void setDay(uint8_t day);

  uint8_t getHour() const;

  void setHour(uint8_t hour);

  uint8_t getMinute() const;

  void setMinute(uint8_t minute);

  uint8_t getSecond() const;

  void setSecond(uint8_t second);

  uint16_t getMillisecond() const;

  void setMillisecond(uint16_t millisecond);

  // operator overloads
  bool operator==(const DateTime &rhs) const;

  bool operator!=(const DateTime &rhs) const;

  bool operator<(const DateTime &rhs) const;

  bool operator>(const DateTime &rhs) const;

  bool operator<=(const DateTime &rhs) const;

  bool operator>=(const DateTime &rhs) const;

  friend std::ostream &operator<<(std::ostream &os, const DateTime &dateTime);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconversion"
  friend std::istream &operator>>(std::istream &is, DateTime &dateTime);
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
