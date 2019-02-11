//
// Created by Petr Flajsingr on 2019-02-08.
//

#ifndef PROJECT_QUERYPARSER_H
#define PROJECT_QUERYPARSER_H

#include <exception>
#include <Exceptions.h>
#include <vector>
#include <string>

namespace DataBase {
class QueryException : public std::exception {
 private:
  const gsl::czstring<> errorMessage;
 public:
  explicit QueryException(const gsl::czstring<> message)
      : errorMessage(message) {}

  char const *what() const noexcept override {
    return errorMessage;
  }
};

class LexException : public QueryException {
 public:
  explicit LexException(const char *message) : QueryException(message) {}
};

class SyntaxException : public QueryException {
 public:
  explicit SyntaxException(const char *message) : QueryException(message) {}
};

class SemanticException : public QueryException {
 public:
  explicit SemanticException(const char *message) : QueryException(message) {}
};

class DataBaseQueryException : public QueryException {
 public:
  explicit DataBaseQueryException(const char *message)
      : QueryException(message) {}
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
  asterisk,
  // keywords:
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

class QueryLexicalAnalyser {
 public:
  void setInput(const std::string &input);

  std::tuple<Token, std::string, bool> getNextToken();
 private:
  std::string input;

  gsl::index currentIndex;

  Token keyWordCheck(std::string_view str);

  std::string getErrorPrint();

  enum class LexState {
    start, num1, numFloat, id, string, exclam, less, greater
  };
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
