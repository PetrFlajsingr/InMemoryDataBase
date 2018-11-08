//
// Created by Petr Flajsingr on 24/08/2018.
//

#include <sstream>
#include <regex>
#include "Utilities.h"

std::vector<std::string> Utilities::splitStringByDelimiter(std::string str,
                                                           std::string delimiter) {
  if (delimiter.empty()) {
    throw InvalidArgumentException("Delimiter can't be empty.");
  }

  std::vector<std::string> result;
  std::string toSplit = std::string(std::move(str));

  size_t pos = 0;
  std::string token;
  while ((pos = toSplit.find(delimiter)) != std::string::npos) {
    token = toSplit.substr(0, pos);
    result.push_back(token);
    toSplit.erase(0, pos + delimiter.length());
  }
  result.push_back(toSplit);

  return result;
}

int Utilities::stringToInt(const std::string &str) {
  std::stringstream stringStream(str);
  int result;

  stringStream >> result;
  return result;
}

double Utilities::stringToDouble(const std::string &str) {
  std::stringstream stringStream(str);
  double result;

  stringStream >> result;
  return result;
}

bool Utilities::isInteger(const std::string &value) {
  std::regex integerRegex("(\\+|-)?[1-9][0-9]*");

  return std::regex_match(value, integerRegex);
}

bool Utilities::isDouble(const std::string &value) {
  std::regex doubleRegex("[+-]?([0-9]*[.])?[0-9]+");

  return std::regex_match(value, doubleRegex);
}

ValueType Utilities::getType(const std::string &value) {
  if (isInteger(value)) {
    return INTEGER_VAL;
  } else if (isDouble(value)) {
    return DOUBLE_VAL;
  } else {
    return STRING_VAL;
  }
}

bool Utilities::endsWith(const std::string &value, const std::string &ending) {
  if (ending.size() > value.size())
    return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

char *Utilities::copyStringToNewChar(const std::string &str) {
  auto result = new char[str.length() + 1];
  strcpy(result, str.c_str());
  return result;
}
