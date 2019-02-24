//
// Created by Petr Flajsingr on 2019-02-23.
//

#ifndef PROJECT_CONSOLEIO_H
#define PROJECT_CONSOLEIO_H

#include <iostream>
#include <string>
#include <string_view>

class ConsoleIO {
 public:
  static void write(std::string_view str);
  static void writeLn(std::string_view str);
  static void writeErr(std::string_view str);
  static void writeLnErr(std::string_view str);
  static std::string readLn();

  enum class Mode {
    simple, arrow
  };

  static void setMode(Mode mode);

 private:
  static std::string form;

  static Mode mode;
};

#endif //PROJECT_CONSOLEIO_H
