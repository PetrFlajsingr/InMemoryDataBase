#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
//
// Created by Petr Flajsingr on 2019-01-25.
//

#ifndef CSV_READER_LOGGER_H
#define CSV_READER_LOGGER_H

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

/**
 * Types of log messages
 */
enum class LogLevel { Verbose, Info, Status, Debug, Warning, Error };

template <bool Debug> class Logger {
private:
  explicit Logger() = default;
  /**
   *
   * @return Current time as HH-MM-SS
   */
  [[nodiscard]] std::string getTime() const {
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
  [[nodiscard]] std::string levelToString(LogLevel level) const {
    switch (level) {
    case LogLevel::Verbose:
      return "";
    case LogLevel::Info:
      return "[INFO]";
    case LogLevel::Status:
      return "[STATUS]";
    case LogLevel::Debug:
      return "[DEBUG]";
    case LogLevel::Warning:
      return "[WARNING]";
    case LogLevel::Error:
      return "[ERROR]";
    }
  }

  std::chrono::milliseconds startTimeMs;
  std::chrono::milliseconds endTimeMs;

public:
  static Logger GetInstance() {
    static Logger instance;
    return instance;
  }
  template <LogLevel Level, bool PrintTime = false, typename... T> void log(T &&... message) const {
    if constexpr (!Debug && Level == LogLevel::Debug) {
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
  void startTime() {
    startTimeMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
  }
  void endTime() {
    endTimeMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
  }

  /**
   * Print time difference between endTime() and startTime() calls
   */
  void printElapsedTime() {
    auto tmp = endTimeMs - startTimeMs;
    log<LogLevel::Verbose>("Time elapsed: " + std::to_string(tmp.count()) + " ms");
  }
};

#endif // CSV_READER_LOGGER_H

#pragma clang diagnostic pop