//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef MISC_HEADERS_TYPES_H_
#define MISC_HEADERS_TYPES_H_

#include <gsl/gsl>
#include <string>
#include <decimal.h>

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
typedef union {
  gsl::zstring<> _string = nullptr;
  int _integer;
  double _double;
  Currency *_currency;
  DateTime *_dateTime;
} DataContainer;

enum class Operation {
  Distinct, Sum, Average
};

std::string OperationToString(Operation op);

#endif  // MISC_HEADERS_TYPES_H_
