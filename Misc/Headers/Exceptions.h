//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef MISC_HEADERS_EXCEPTIONS_H_
#define MISC_HEADERS_EXCEPTIONS_H_

#include <stdexcept>
#include <string>
#include <gsl/gsl>

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
  const gsl::czstring<> errorMessage;
 public:
  explicit IOException(const gsl::czstring<> message) : errorMessage(message) {}

  char const *what() const noexcept override {
    return errorMessage;
  }
};

/**
 * Exception pro informovani o chybne zadanych argumentech
 */
class InvalidArgumentException : public std::invalid_argument {
 public:
  explicit InvalidArgumentException(const gsl::czstring<> message)
      : invalid_argument(
      message) {}
};

/**
 * Exception pro notifikaci o chybnem pozadavku pro momentalni vnitrni stav objektu
 */
class IllegalStateException : public std::exception {
 private:
  const gsl::czstring<> errorMessage;
 public:
  explicit IllegalStateException(const gsl::czstring<> message) : errorMessage(
      message) {}

  char const *what() const noexcept override {
    return errorMessage;
  }
};

/**
 * Exception pro notifikaci o nepovolene operaci objektu
 */
class UnsupportedOperationException : public std::exception {
 private:
  const gsl::czstring<> errorMessage;
 public:
  explicit UnsupportedOperationException(const gsl::czstring<> message)
      : errorMessage(message) {}

  char const *what() const noexcept override {
    return errorMessage;
  }
};

class DataBaseException : public std::exception {
 private:
  const gsl::czstring<> errorMessage;
 public:
  explicit DataBaseException(const std::string &errorMessage) : errorMessage(
      errorMessage.c_str()) {}
  explicit DataBaseException(const gsl::czstring<> message)
      : errorMessage(message) {}

  char const *what() const noexcept override {
    return errorMessage;
  }
};

class ResourceNotFoundException : public std::exception {
 private:
  const gsl::czstring<> errorMessage;
 public:
  explicit ResourceNotFoundException(const std::string &errorMessage)
      : errorMessage(
      errorMessage.c_str()) {}
  explicit ResourceNotFoundException(const gsl::czstring<> message)
      : errorMessage(message) {}

  char const *what() const noexcept override {
    return errorMessage;
  }
};
#endif  //  MISC_HEADERS_EXCEPTIONS_H_
