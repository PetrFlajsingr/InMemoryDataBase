//
// Created by Petr Flajsingr on 2019-02-08.
//

#ifndef PROJECT_QUERYPARSER_H
#define PROJECT_QUERYPARSER_H

#include <exception>
#include <Exceptions.h>
#include <vector>
namespace DataBase {
class QueryException : public std::exception {

};

class LexException : public QueryException {

};

class SyntaxException : public QueryException {

};

class SemanticException : public QueryException {

};

class DataBaseQueryException : public QueryException, public DataBaseException {

};

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
  asterisk,
  // keywords:
      select,
  from,
  where,
  orderBy,
  join,
  leftJoin,
  rightJoin,
  outerJoin,
  having,
  sum,
  avg,
  min,
  max,
  count,
  asc,
  desc,
  orLogic,
  andLogic
};

class QueryLexicalAnalyser {
 public:
  std::vector<Token> analyseCharSequence(std::string_view input);
 private:
};

class QuerySyntaxAnalyser {
 public:
  StructuredQuery analyseTokenSequence(const std::vector<Token> &input);
 private:

};

class QuerySemanticAnalyser {
 public:
  void analyseStructuredQuery(const StructuredQuery &structuredQuery);
 private:

};

class QueryAnalyser {
 public:
  StructuredQuery analyseQuery(std::string_view);

 private:
  QueryLexicalAnalyser lexicalAnalyser;
  QuerySyntaxAnalyser syntaxAnalyser;
  QuerySemanticAnalyser semanticAnalyser;
};
}  // namespace DataBase

#endif //PROJECT_QUERYPARSER_H
