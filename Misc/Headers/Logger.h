//
// Created by Petr Flajsingr on 2019-01-25.
//

#ifndef CSV_READER_LOGGER_H
#define CSV_READER_LOGGER_H

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

enum LogLevel { Verbose, Info, Status, Debug, Warning, Error };

class Logger {
 private:

  static std::string getTime() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%H-%M-%S");
    return ss.str();
  }

  static std::string levelToString(LogLevel level) {
    switch (level) {
      case Verbose:return "";
      case Info: return "[INFO]";
      case Status: return "[STATUS]";
      case Debug: return "[DEBUG]";
      case Warning: return "[WARNING]";
      case Error: return "[ERROR]";
    }
    return "";
  }

 public:

  static void log(LogLevel logLevel, std::string message, bool printTime = false) {
    if (printTime) {
      message = levelToString(logLevel) + " " + getTime() + ": " + message;
    } else {
      message = levelToString(logLevel) + ": " + message;
    }

    std::cout << message << std::endl;
  }

  static void log(LogLevel logLevel, const std::exception &exception, bool printTime = false) {
    std::string message = exception.what();
    if (printTime) {
      message = levelToString(logLevel) + " " + getTime() + ": " + message;
    } else {
      message = levelToString(logLevel) + ": " + message;
    }

    std::cout << message << std::endl;
  }
};

#endif //CSV_READER_LOGGER_H
