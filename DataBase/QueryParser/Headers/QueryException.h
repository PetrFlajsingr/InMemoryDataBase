//
// Created by Petr Flajsingr on 2019-02-12.
//

#ifndef PROJECT_QUERYEXCEPTION_H
#define PROJECT_QUERYEXCEPTION_H

#include <exception>
#include <gsl/gsl>

namespace DataBase {
class QueryException : public std::exception {
 private:
  const std::string errorMessage;
 public:
  explicit QueryException(const std::string &message) : errorMessage(message) {}
  char const *what() const noexcept override {
    return errorMessage.c_str();
  }
};

class LexException : public QueryException {
 public:
  explicit LexException(const std::string &message) : QueryException(message) {}
};

class SyntaxException : public QueryException {
 public:
  explicit SyntaxException(const std::string &message) : QueryException(message) {}
};

class SemanticException : public QueryException {
 public:
  explicit SemanticException(const std::string &message) : QueryException(message) {}
};

class DataBaseQueryException : public QueryException {
 public:
  explicit DataBaseQueryException(const std::string &message) : QueryException(message) {}
};
}

#endif //PROJECT_QUERYEXCEPTION_H
