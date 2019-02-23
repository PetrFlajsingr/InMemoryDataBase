//
// Created by Petr Flajsingr on 2019-02-23.
//

#include <ConsoleIO.h>

void ConsoleIO::write(std::string_view str) {
  std::cout << str;
}
void ConsoleIO::writeLn(std::string_view str) {
  std::cout << str << std::endl;
}
void ConsoleIO::writeErr(std::string_view str) {
  std::cerr << str;
}
void ConsoleIO::writeLnErr(std::string_view str) {
  std::cerr << str << std::endl;
}
std::string ConsoleIO::readLn() {
  std::string str;
  std::cin >> str;
  return str;
}
