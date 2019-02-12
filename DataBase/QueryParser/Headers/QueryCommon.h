//
// Created by Petr Flajsingr on 2019-02-12.
//

#ifndef PROJECT_QUERYCOMMON_H
#define PROJECT_QUERYCOMMON_H

#include <string>
#include <vector>
#include <utility>

namespace DataBase {

enum class CondOperator {
  greater, greaterEqual, less, lessEqual, equal, notEqual
};

enum class LogicOperator {
  logicOr, logicAnd, none
};

enum class ConstType {
  integer, floatingPoint, string
};

enum class AgrOperator {
  sum, avg, count, min, max, group
};

enum class OrderDir {
  asc, desc
};

struct FieldId {
  std::string table;
  std::string column;
};

struct WhereItem {
  FieldId field;
  CondOperator condOperator;
  std::vector<std::pair<ConstType, std::string>> valuesConst;
  std::vector<FieldId> values;
};
struct WhereStructure {
  std::vector<std::pair<WhereItem, LogicOperator>> data;
};

struct JoinItem {
  std::string joinedTable;
  FieldId firstField;
  FieldId secondField;
};
struct JoinStructure {
  std::vector<JoinItem> data;
};

struct AgreItem {
  AgrOperator op;
  FieldId field;
};
struct AgreStructure {
  std::vector<AgreItem> data;
};

struct HavingItem {
  AgrOperator op;
  FieldId field;
  std::vector<std::pair<ConstType, std::string>> values;
};
struct HavingStructure {
  std::vector<HavingItem> data;
};

struct OrderItem {
  OrderDir order;
  FieldId field;
};
struct OrderStructure {
  std::vector<OrderItem> data;
};

struct ProjectItem : public FieldId {};
struct ProjectStructure {
  std::vector<ProjectItem> data;
};

struct StructuredQuery {
  WhereStructure where;
  JoinStructure joins;
  AgreStructure agr;
  HavingStructure having;
  OrderStructure order;
  ProjectStructure project;
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
  logicOr,
  logicAnd,
  on
};

std::string tokenToString(Token token);

CondOperator tokenToCondOperator(Token token);

ConstType tokenToConstType(Token token);

LogicOperator tokenToLogic(Token token);

AgrOperator tokenToAgrOperation(Token token);
}

#endif //PROJECT_QUERYCOMMON_H
