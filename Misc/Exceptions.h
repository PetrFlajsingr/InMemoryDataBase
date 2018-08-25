//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef CSV_READER_EXCEPTIONS_H
#define CSV_READER_EXCEPTIONS_H

#include <stdexcept>
#include <string>

/**
 * Exception sloužící k označení funkcí, které nejsou implementovány
 */
class NotImplementedException : public std::logic_error {
public:
    NotImplementedException() : logic_error("Function is not implemented"){}
};

/**
 * Exception sloužící k informování o chybné práci se soubory
 */
class IOException : public std::exception {
private:
    const char *errorMessage;
public:
    explicit IOException(const char* message) {
        this->errorMessage = message;
    }

    char const* what() const throw() override {
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
    explicit InvalidArgumentException(const char* message) {
        this->errorMessage = message;
    }

    char const* what() const throw() override {
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
    explicit IllegalStateException(const char* message) {
        this->errorMessage = message;
    }

    char const* what() const throw() override {
        return errorMessage;
    }
};

#endif //CSV_READER_EXCEPTIONS_H
