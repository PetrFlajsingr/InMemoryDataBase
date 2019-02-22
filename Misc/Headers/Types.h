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

class DateTime;
/**
 * Typy ulozenych dat
 */
enum class ValueType {
  None,
  Integer,
  Double,
  String,
  Currency,
  DateTime
};

/**
 * Poradi pro razeni
 */
enum class SortOrder {
  Ascending,
  Descending
};

/**
 * Mena reprezentovana jako fixed point hodnota se dvemi desetinnymi misty
 */
typedef dec::decimal<2> Currency;

/**
 * Union ukladajici data memory data setu.
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
  DataContainer &operator=(Currency &val);
  DataContainer &operator=(DateTime &val);
  template<typename T>
  DataContainer &operator=(T) { return *this; }

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

/**
 * Struktura pro jeden zaznam v data setu.
 */
using DataSetRow = std::vector<DataContainer>;

int compareDataContainers(const DataContainer &data1,
                          const DataContainer &data2,
                          ValueType valueType);

enum class Operation {
  Distinct, Sum, Average
};

std::string OperationToString(Operation op);

#endif  // MISC_HEADERS_TYPES_H_
