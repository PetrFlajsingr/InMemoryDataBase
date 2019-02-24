//
// Created by Petr Flajsingr on 2019-02-23.
//

#include <ConsoleIO.h>

ConsoleIO::Mode ConsoleIO::mode = ConsoleIO::Mode::simple;
std::string ConsoleIO::form;

void ConsoleIO::write(std::string_view str) {
  std::cout << ConsoleIO::form << str;
}
void ConsoleIO::writeLn(std::string_view str) {
  std::cout << ConsoleIO::form << str << std::endl;
  if (mode == Mode::arrow) {
    std::cout << ConsoleIO::form;
  }
}
void ConsoleIO::writeErr(std::string_view str) {
  std::cerr << ConsoleIO::form << str;
}
void ConsoleIO::writeLnErr(std::string_view str) {
  std::cerr << ConsoleIO::form << str << std::endl;
}
std::string ConsoleIO::readLn() {
  std::string str;
  std::getline(std::cin, str);
  return str;
}
void ConsoleIO::setMode(ConsoleIO::Mode mode) {
  ConsoleIO::mode = mode;
  switch (mode) {
    case Mode::simple:ConsoleIO::form = "";
      break;
    case Mode::arrow:ConsoleIO::form = ">> ";
      write("");
      break;
  }
}
