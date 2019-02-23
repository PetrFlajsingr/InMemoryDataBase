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
};

#endif //PROJECT_CONSOLEIO_H
