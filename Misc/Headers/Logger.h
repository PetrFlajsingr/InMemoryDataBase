//
// Created by Petr Flajsingr on 2019-01-25.
//

#ifndef CSV_READER_LOGGER_H
#define CSV_READER_LOGGER_H

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>

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

  inline static std::chrono::milliseconds startTimeMs;
  inline static std::chrono::milliseconds endTimeMs;

 public:

  static void log(LogLevel logLevel, std::string message, bool printTime = false) {
    if (logLevel != Verbose) {
      if (printTime) {
        message = levelToString(logLevel) + " " + getTime() + ": " + message;
      } else {
        message = levelToString(logLevel) + ": " + message;
      }
    }

    std::cout << message << std::endl;
  }

  static void log(LogLevel logLevel, const std::exception &exception, bool printTime = false) {
    std::string message = exception.what();
    if (logLevel != Verbose) {
      if (printTime) {
        message = levelToString(logLevel) + " " + getTime() + ": " + message;
      } else {
        message = levelToString(logLevel) + ": " + message;
      }
    }

    std::cout << message << std::endl;
  }

  static void startTime() {
    Logger::startTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
  }

  static void endTime() {
    Logger::endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
  }

  static void printElapsedTime() {
    auto tmp = Logger::endTimeMs - Logger::startTimeMs;
    Logger::log(Verbose, "Time elapsed: " + std::to_string(tmp.count()) + " ms");
  }
};

#endif //CSV_READER_LOGGER_H
