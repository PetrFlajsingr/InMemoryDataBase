//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef MISC_HEADERS_TYPES_H_
#define MISC_HEADERS_TYPES_H_

#include <Utilities.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <decimal.h>
#include <gsl/gsl>
#include <iostream>
#include <string>
#include <vector>
#include <xlnt/utils/date.hpp>
#include <xlnt/utils/datetime.hpp>
#include <xlnt/utils/time.hpp>

enum class DateTimeType { Date, Time, DateTime };
/**
 * DateTime wrapper for boost/date_time.
 * Provides methods for string conversions and conversions for xlnt library.
 */
class DateTime {
public:
  DateTime();
  explicit DateTime(DateTimeType type);
  DateTime(const boost::posix_time::ptime &ptime, DateTimeType type);
  DateTime(const DateTime &other);
  /**
   * Construct from string using default formats.
   * @param str date/time/datetime string
   * @param type
   */
  DateTime(std::string_view str, DateTimeType type);
  /**
   * Construct from string using user defined format.
   * @param str date/time/datetime string
   * @param fmt format of string
   */
  DateTime(std::string_view str, std::string_view fmt);

  [[nodiscard]] DateTimeType getType() const;
  /**
   * Convert to string using default formats.
   * @return string representation of this
   */
  [[nodiscard]] std::string toString() const;
  /**
   * Convert to string using user defined format.
   * @return string representation of this
   */
  [[nodiscard]] std::string toString(std::string_view fmt) const;

  /**
   * Set datetime from string using default formats.
   * @param str datetime as string
   */
  void fromString(std::string_view str);
  /**
   * Set datetime from string using user defined format.
   * @param str datetime as string
   * @param fmt datetime string format
   */
  void fromString(std::string_view str, std::string_view fmt);

  xlnt::date toXlntDate();
  xlnt::time toXlntTime();
  xlnt::datetime toXlntDateTime();

  [[nodiscard]] const boost::posix_time::ptime &getTime() const;

  bool operator==(const DateTime &rhs) const;
  bool operator!=(const DateTime &rhs) const;
  bool operator<(const DateTime &rhs) const;
  bool operator>(const DateTime &rhs) const;
  bool operator<=(const DateTime &rhs) const;
  bool operator>=(const DateTime &rhs) const;
  friend std::ostream &operator<<(std::ostream &os, const DateTime &timeB);
  friend std::istream &operator>>(std::istream &is, DateTime &timeB);

  static const std::string dateTimeDefFmt;
  static const std::string dateDefFmt;
  static const std::string timeDefFmt;
private:
  DateTimeType type;

  boost::posix_time::ptime ptime;

  static std::pair<DateTimeType, boost::posix_time::ptime> innerFromString(std::string_view str, std::string_view fmt);
};

enum class ValueType { Integer, Double, String, Currency, DateTime };

enum class SortOrder { Ascending, Descending };

typedef dec::decimal<2> Currency;

/**
 * Main data container for data sets.
 */
union DataContainer {
  gsl::czstring<> _string = nullptr;
  int _integer;
  double _double;
  Currency *_currency;
  DateTime *_dateTime;

  explicit operator gsl::czstring<>() const { return _string; }
  explicit operator int() const { return _integer; }
  explicit operator double() const { return _double; }
  explicit operator Currency() const { return *_currency; }
  explicit operator DateTime() const { return *_dateTime; }
  template <typename T> explicit operator T() const { return *this; }

  DataContainer &operator=(gsl::czstring<> val);
  DataContainer &operator=(int val);
  DataContainer &operator=(double val);
  DataContainer &operator=(const Currency &val);
  DataContainer &operator=(const DateTime &val);

  template <typename T> bool operator==(T &rhs) {
    if constexpr (std::is_same_v<T, int>) {
      return Utilities::compareInt(_integer, rhs);
    }
    if constexpr (std::is_same<T, double>{}) {
      return Utilities::compareDouble(_double, rhs);
    }
    if constexpr (std::is_same<T, gsl::zstring<>>{}) {
      return Utilities::compareString(_string, rhs);
    }
    if constexpr (std::is_same<T, Currency>{}) {
      return Utilities::compareCurrency(*_currency, rhs);
    }
    if constexpr (std::is_same<T, DateTime>{}) {
      return Utilities::compareDateTime(*_dateTime, rhs);
    }
    throw IllegalStateException("DataContainer::operator==()): invalid type");
  }

  template <typename T>[[nodiscard]] int compare(const DataContainer &val) const {
    if constexpr (std::is_same<T, int>{}) {
      return Utilities::compareInt(_integer, val._integer);
    }
    if constexpr (std::is_same<T, double>{}) {
      return Utilities::compareDouble(_double, val._double);
    }
    if constexpr (std::is_same<T, gsl::zstring<>>{}) {
      return Utilities::compareString(_string, val._string);
    }
    if constexpr (std::is_same<T, Currency>{}) {
      return Utilities::compareCurrency(*_currency, *val._currency);
    }
    if constexpr (std::is_same<T, DateTime>{}) {
      return Utilities::compareDateTime(*_dateTime, *val._dateTime);
    }
  }

  template <typename T> void copyFrom(DataContainer &val) {
    if constexpr (std::is_same<T, int>{}) {
      _integer = val._integer;
    }
    if constexpr (std::is_same<T, double>{}) {
      _double = val._double;
    }
    if constexpr (std::is_same<T, gsl::zstring<>>{}) {
      _string = val._string;
    }
    if constexpr (std::is_same<T, Currency>{}) {
      *_currency = *val._currency;
    }
    if constexpr (std::is_same<T, DateTime>{}) {
      *_dateTime = *val._dateTime;
    }
  }
};

using DataSetRow = std::vector<DataContainer>;

int compareDataContainers(const DataContainer &data1, const DataContainer &data2, ValueType valueType);

#endif // MISC_HEADERS_TYPES_H_
