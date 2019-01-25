//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef MISC_HEADERS_TYPES_H_
#define MISC_HEADERS_TYPES_H_

#include "decimal.h"

/**
 * Typy ulozenych dat
 */
enum ValueType { NONE, INTEGER_VAL, DOUBLE_VAL, STRING_VAL, CURRENCY_VAL };

enum SortOrder { ASCENDING, DESCENDING };

typedef dec::decimal<2> Currency;

/**
 * Union ukladajici data v pameti.
 */
typedef union {
  char *_string = nullptr;
  int _integer;
  double _double;
  Currency *_currency;
} DataContainer;

#endif  // MISC_HEADERS_TYPES_H_
