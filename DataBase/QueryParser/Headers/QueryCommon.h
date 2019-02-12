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

enum class Order {
  asc, desc
};

enum class JoinType {
  innerJoin, leftJoin, rightJoin, outerJoin
};

struct FieldId {
  std::string table;
  std::string column;
};

struct WhereItem {
  FieldId field;
  CondOperator condOperator;
  std::vector<std::pair<ConstType, std::string>> constValues;
  std::vector<FieldId> values;
};
struct WhereStructure {
  std::vector<std::pair<WhereItem, LogicOperator>> data;
};

struct JoinItem {
  std::string joinedTable;
  FieldId firstField;
  FieldId secondField;
  JoinType type;
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
  AgreItem agreItem;
  CondOperator condOperator;
  std::vector<std::pair<ConstType, std::string>> constValues;
  std::vector<FieldId> values;
};
struct HavingStructure {
  std::vector<std::pair<HavingItem, LogicOperator>> data;
};

struct OrderItem {
  Order order;
  FieldId field;
};
struct OrderStructure {
  std::vector<OrderItem> data;
};

using ProjectItem = FieldId;
struct ProjectStructure {
  std::vector<ProjectItem> data;
};

struct StructuredQuery {
  std::string mainTable;
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
  on,
  as,
  unknown
};

std::string tokenToString(Token token);

CondOperator tokenToCondOperator(Token token);

ConstType tokenToConstType(Token token);

LogicOperator tokenToLogic(Token token);

AgrOperator tokenToAgrOperation(Token token);

Order tokenToOrder(Token token);
}

#endif //PROJECT_QUERYCOMMON_H
