#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
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

/**
 * Types of log messages
 */
enum class LogLevel { Verbose, Info, Status, Debug, Warning, Error };

class Logger {
 private:
  explicit Logger(bool isAllowedDebug = false)
      : isAllowedDebug(isAllowedDebug) {}
  /**
   *
   * @return Current time as HH-MM-SS
   */
  std::string getTime() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%H-%M-%S");
    return ss.str();
  }
  /**
   * Log tags
   * @param level
   * @return
   */
  std::string levelToString(LogLevel level) {
    switch (level) {
      case LogLevel::Verbose:return "";
      case LogLevel::Info: return "[INFO]";
      case LogLevel::Status: return "[STATUS]";
      case LogLevel::Debug: return "[DEBUG]";
      case LogLevel::Warning: return "[WARNING]";
      case LogLevel::Error: return "[ERROR]";
    }
  }

  bool isAllowedDebug;

  std::chrono::milliseconds startTimeMs;
  std::chrono::milliseconds endTimeMs;

 public:
  static Logger GetInstance() {
    static Logger instance;
    return instance;
  }
  /**
   * Print to stdout
   * @param logLevel
   * @param message
   * @param printTime
   */
  void log(LogLevel logLevel, std::string message, bool printTime = false) {
    if (isAllowedDebug && logLevel == LogLevel::Debug) {
      return;
    }
    if (logLevel != LogLevel::Verbose) {
      if (printTime) {
        message = levelToString(logLevel) + " " + getTime() + ": " + message;
      } else {
        message = levelToString(logLevel) + ": " + message;
      }
    }
    std::cout << message << std::endl;
  }
  /**
   * Exception logging
   * @param logLevel
   * @param exception exception to print out
   * @param printTime
   */
  void log(LogLevel logLevel,
           const std::exception &exception,
           bool printTime = false) {
    if (isAllowedDebug && logLevel == LogLevel::Debug) {
      return;
    }
    std::string message = exception.what();
    if (logLevel != LogLevel::Verbose) {
      if (printTime) {
        message = levelToString(logLevel) + " " + getTime() + ": " + message;
      } else {
        message = levelToString(logLevel) + ": " + message;
      }
    }
    std::cout << message << std::endl;
  }
  void startTime() {
    startTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
  }
  void endTime() {
    endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
  }

  /**
   * Print time difference between endTime() and startTime() calls
   */
  void printElapsedTime() {
    auto tmp = endTimeMs - startTimeMs;
    log(LogLevel::Verbose,
        "Time elapsed: " + std::to_string(tmp.count()) + " ms");
  }

  void allowDebug() {
    isAllowedDebug = true;
  }
};

#endif //CSV_READER_LOGGER_H

#pragma clang diagnostic pop