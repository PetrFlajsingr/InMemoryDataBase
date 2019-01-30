//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef MISC_HEADERS_TYPES_H_
#define MISC_HEADERS_TYPES_H_

#include <decimal.h>

class DateTime;
/**
 * Typy ulozenych dat
 */
enum ValueType {
  NoneValue,
  IntegerValue,
  DoubleValue,
  StringValue,
  CurrencyValue,
  DateTimeValue
};

/**
 * Poradi pro razeni
 */
enum SortOrder { Ascending, Descending };

/**
 * Mena reprezentovana jako fixed point hodnota se dvemi desetinnymi misty
 */
typedef dec::decimal<2> Currency;

/**
 * Union ukladajici data memory data setu.
 */
typedef union {
  char *_string = nullptr;
  int _integer;
  double _double;
  Currency *_currency;
  DateTime *_dateTime;
} DataContainer;

#endif  // MISC_HEADERS_TYPES_H_
