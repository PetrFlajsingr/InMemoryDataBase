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
}

#endif //PROJECT_QUERYEXCEPTION_H
