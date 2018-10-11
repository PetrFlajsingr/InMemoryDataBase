//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef MISC_HEADERS_EXCEPTIONS_H_
#define MISC_HEADERS_EXCEPTIONS_H_

#include <stdexcept>
#include <string>

/**
 * Exception slouzici k oznaceni funkci, ktere nejsou implementovany
 */
class NotImplementedException : public std::logic_error {
 public:
  NotImplementedException() : logic_error("Function is not implemented") {}
};

/**
 * Exception slouzici k informovani o chybné praci se soubory
 */
class IOException : public std::exception {
 private:
  const char *errorMessage;
 public:
  explicit IOException(const char *message) {
    this->errorMessage = message;
  }

  char const *what() const noexcept override {
    return errorMessage;
  }
};

/**
 * Exception pro informovani o chybne zadanych argumentech
 */
class InvalidArgumentException : public std::exception {
 private:
  const char *errorMessage;
 public:
  explicit InvalidArgumentException(const char *message) {
    this->errorMessage = message;
  }

  char const *what() const noexcept override {
    return errorMessage;
  }
};

/**
 * Exception pro notifikaci o chybnem pozadavku pro momentalni vnitrni stav objektu
 */
class IllegalStateException : public std::exception {
 private:
  const char *errorMessage;
 public:
  explicit IllegalStateException(const char *message) {
    this->errorMessage = message;
  }

  char const *what() const noexcept override {
    return errorMessage;
  }
};

/**
 * Exception pro notifikaci o nepovolene operaci objektu
 */
class UnsupportedOperationException : public std::exception {
 private:
  const char *errorMessage;
 public:
  explicit UnsupportedOperationException(const char *message) {
    this->errorMessage = message;
  }

  char const *what() const noexcept override {
    return errorMessage;
  }
};
#endif //  MISC_HEADERS_EXCEPTIONS_H_
