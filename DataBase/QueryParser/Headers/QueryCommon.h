//
// Created by Petr Flajsingr on 2019-02-12.
//

#ifndef PROJECT_QUERYCOMMON_H
#define PROJECT_QUERYCOMMON_H

#include <string>

namespace DataBase {

class StructuredQuery {
  // where
  // join
  // agre (+ group by)
  // having
  // order by
  // proje
};

enum class Token {
  id,
  number,
  numberFloat,
  string,
  equal,
  notEqual,
  greater,
  greaterEqual,
  less,
  lessEqual,
  leftBracket,
  rightBracket,
  dot,
  colon,
  semicolon,
  pipe,
  space,
  asterisk, // keywords:
  select,
  group,
  from,
  where,
  order,
  by,
  join,
  left,
  right,
  outer,
  having,
  sum,
  avg,
  min,
  max,
  count,
  asc,
  desc,
  orLogic,
  andLogic,
  on
};

std::string tokenToString(Token token);

}

#endif //PROJECT_QUERYCOMMON_H
