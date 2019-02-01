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
 * Typ zpravy pro log
 */
enum class LogLevel { Verbose, Info, Status, Debug, Warning, Error };

class Logger {
 private:
  explicit Logger(bool isAllowedDebug = false)
      : isAllowedDebug(isAllowedDebug) {}

  /**
   *
   * @return Momentalni cas ve formatu HH-MM-SS
   */
  std::string getTime() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%H-%M-%S");
    return ss.str();
  }

  /**
   * Popisek pro jednotlive urovne logovani
   * @param level
   * @return
   */
  std::string levelToString(LogLevel level) {
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

  bool isAllowedDebug;

  /**
   * Pro logovani casu
   */
  std::chrono::milliseconds startTimeMs;
  std::chrono::milliseconds endTimeMs;

 public:
  static Logger getInstance() {
    static Logger instance;
    return instance;
  }

  /**
   * Vypis zpravu na stdout
   * @param logLevel typ zpravy
   * @param message zprava
   * @param printTime pokud true, vypise cas, jinak nic
   */
  void log(LogLevel logLevel, std::string message, bool printTime = false) {
    if (isAllowedDebug && logLevel == LogLevel::Debug) {
      return;
    }
    if (logLevel != Verbose) {
      if (printTime) {
        message = levelToString(logLevel) + " " + getTime() + ": " + message;
      } else {
        message = levelToString(logLevel) + ": " + message;
      }
    }
    std::cout << message << std::endl;
  }

  /**
   * Vypis obsahu .what() ve vyjimce
   * @param logLevel typ zpravy
   * @param exception vyjimka, o niz se ma vypsat info
   * @param printTime pokud true, vypise cas, jinak nic
   */
  void log(LogLevel logLevel,
           const std::exception &exception,
           bool printTime = false) {
    if (isAllowedDebug && logLevel == LogLevel::Debug) {
      return;
    }
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

  /**
   * Uloz pocatecni stav pro logovani.
   */
  void startTime() {
    Logger::startTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
  }

  /**
   * Uloz koncovy stav pro logovani.
   */
  void endTime() {
    Logger::endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
  }

  /**
   * Vypise na stdout rozdil endTime a startTime v ms.
   */
  void printElapsedTime() {
    auto tmp = Logger::endTimeMs - Logger::startTimeMs;
    Logger::log(Verbose,
                "Time elapsed: " + std::to_string(tmp.count()) + " ms");
  }

  void allowDebug() {
    isAllowedDebug = true;
  }
};

#endif //CSV_READER_LOGGER_H
