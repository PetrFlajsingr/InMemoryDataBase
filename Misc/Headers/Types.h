//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef MISC_HEADERS_TYPES_H_
#define MISC_HEADERS_TYPES_H_

#include <gsl/gsl>
#include <string>
#include <decimal.h>
#include <DateTimeUtils.h>
#include <Utilities.h>
#include <iostream>
#include <Property.h>

enum class DateTimeType {
  Date, Time, DateTime
};
class DateTimeB {
 public:
  DateTimeB(const boost::posix_time::ptime &ptime, DateTimeType type);

  DateTimeType getType() const;
  std::string toString() const;
  std::string toString(std::string_view fmt) const;

  const boost::posix_time::ptime &getTime() const;

  // TODO: comparison operators, copy constructor...

  static DateTimeB fromString(std::string_view str, DateTimeType type);
  static DateTimeB fromString(std::string_view str, std::string_view fmt);
 private:
  DateTimeType type;
  boost::posix_time::ptime ptime;

  static const std::string dateTimeDefFmt;
  static const std::string dateDefFmt;
  static const std::string timeDefFmt;
};

class DateTime;

enum class ValueType {
  Integer,
  Double,
  String,
  Currency,
  DateTime
};

enum class SortOrder {
  Ascending,
  Descending
};

typedef dec::decimal<2> Currency;

/**
 * Main data container for data sets
 */
union DataContainer {
  gsl::zstring<> _string = nullptr;
  int _integer;
  double _double;
  Currency *_currency;
  DateTime *_dateTime;

  explicit operator gsl::zstring<>() const { return _string; }
  explicit operator int() const { return _integer; }
  explicit operator double() const { return _double; }
  explicit operator Currency() const { return *_currency; }
  explicit operator DateTime() const { return *_dateTime; }
  template<typename T>
  explicit operator T() const { return *this; }

  DataContainer &operator=(gsl::zstring<> val);
  DataContainer &operator=(int val);
  DataContainer &operator=(double val);
  DataContainer &operator=(const Currency &val);
  DataContainer &operator=(const DateTime &val);

  template<typename T>
  bool operator==(T &rhs) {
    if constexpr (std::is_same<T, int>{}) {
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
  }

  template<typename T>
  int compare(const DataContainer &val) const {
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

  template<typename T>
  void copyFrom(DataContainer &val) {
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

int compareDataContainers(const DataContainer &data1,
                          const DataContainer &data2,
                          ValueType valueType);

enum class Operation {
  Distinct, Sum, Average
};

std::string OperationToString(Operation op);

#endif  // MISC_HEADERS_TYPES_H_
