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
enum LogLevel { Verbose, Info, Status, Debug, Warning, Error };

// TODO: povoleni/zakaz vypis Debug, Verbose
class Logger {
 private:
  /**
   *
   * @return Momentalni cas ve formatu HH-MM-SS
   */
  static std::string getTime() {
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

  /**
   * Pro logovani casu
   */
  inline static std::chrono::milliseconds startTimeMs;
  inline static std::chrono::milliseconds endTimeMs;

 public:

  /**
   * Vypis zpravu na stdout
   * @param logLevel typ zpravy
   * @param message zprava
   * @param printTime pokud true, vypise cas, jinak nic
   */
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

  /**
   * Vypis obsahu .what() ve vyjimce
   * @param logLevel typ zpravy
   * @param exception vyjimka, o niz se ma vypsat info
   * @param printTime pokud true, vypise cas, jinak nic
   */
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

  /**
   * Uloz pocatecni stav pro logovani.
   */
  static void startTime() {
    Logger::startTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
  }

  /**
   * Uloz koncovy stav pro logovani.
   */
  static void endTime() {
    Logger::endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
  }

  /**
   * Vypise na stdout rozdil endTime a startTime v ms.
   */
  static void printElapsedTime() {
    auto tmp = Logger::endTimeMs - Logger::startTimeMs;
    Logger::log(Verbose, "Time elapsed: " + std::to_string(tmp.count()) + " ms");
  }
};

#endif //CSV_READER_LOGGER_H
