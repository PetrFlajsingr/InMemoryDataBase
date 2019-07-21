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

template <bool Debug>
class Logger {
 private:
  explicit Logger() = default;
  /**
   *
   * @return Current time as HH-MM-SS
   */
  std::string getTime() const {
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
  std::string levelToString(LogLevel level) const {
    switch (level) {
      case LogLevel::Verbose:return "";
      case LogLevel::Info: return "[INFO]";
      case LogLevel::Status: return "[STATUS]";
      case LogLevel::Debug: return "[DEBUG]";
      case LogLevel::Warning: return "[WARNING]";
      case LogLevel::Error: return "[ERROR]";
    }
  }

  std::chrono::milliseconds startTimeMs;
  std::chrono::milliseconds endTimeMs;

 public:
  static Logger GetInstance() {
    static Logger instance;
    return instance;
  }
  template <LogLevel Level, bool PrintTime = false, typename ...T>
  void log(T&&... message) const {
    if constexpr (Debug && Level == LogLevel::Debug) {
      return;
    }
    if constexpr (Level != LogLevel::Verbose) {
      if constexpr (PrintTime) {
        std::cout << levelToString(Level) + " " + getTime() + ": ";
      } else {
        std::cout << levelToString(Level) + ": ";
      }
    }
    (std::cout << ... << message) << std::endl;
  }
  /**
   * Print to stdout
   * @param logLevel
   * @param message
   * @param printTime
   */
  /*void log(LogLevel logLevel, std::string message, bool printTime = false) {
    if (Debug && logLevel == LogLevel::Debug) {
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
  }*/
  /**
   * Exception logging
   * @param logLevel
   * @param exception exception to print out
   * @param printTime
   */
  /*void log(LogLevel logLevel, const std::exception &exception, bool printTime = false) {
    if (Debug && logLevel == LogLevel::Debug) {
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
  }*/
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
    log(LogLevel::Verbose, "Time elapsed: " + std::to_string(tmp.count()) + " ms");
  }
};

#endif //CSV_READER_LOGGER_H

#pragma clang diagnostic pop