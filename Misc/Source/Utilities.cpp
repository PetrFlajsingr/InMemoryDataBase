//
// Created by Petr Flajsingr on 24/08/2018.
//

#include <sstream>
#include <regex>
#include <Utilities.h>
#include "DateTimeUtils.h"

std::vector<std::string> Utilities::splitStringByDelimiter(std::string str,
                                                           std::string delimiter) {
  std::vector<std::string> result;

  size_t pos = 0;
  while ((pos = str.find(delimiter)) != std::string::npos) {
    result.emplace_back(std::move(str.substr(0, pos)));
    str.erase(0, pos + delimiter.length());
  }
  result.emplace_back(std::move(str));

  return result;
}

std::vector<std::string> Utilities::splitStringByDelimiterReserve(std::string str,
                                                                  std::string delimiter,
                                                                  int reserve) {
  std::vector<std::string> result;
  result.reserve(reserve);

  size_t pos = 0;
  while ((pos = str.find(delimiter)) != std::string::npos) {
    result.emplace_back(str.substr(0, pos));
    str.erase(0, pos + delimiter.length());
  }
  result.emplace_back(str);

  return result;
}

int Utilities::stringToInt(const std::string &str) {
  gsl::zstring<> p;
  long converted = strtol(str.c_str(), &p, 10);
  if (*p) {
    return 0;
  } else {
    return converted;
  }
}

double Utilities::stringToDouble(const std::string &str) {
  gsl::zstring<> p;
  double converted = strtod(str.c_str(), &p);
  if (*p) {
    return 0;
  } else {
    return converted;
  }
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
    return ValueType::Integer;
  } else if (isDouble(value)) {
    return ValueType::Double;
  } else {
    return ValueType::String;
  }
}

bool Utilities::endsWith(const std::string &value, const std::string &ending) {
  if (ending.size() > value.size())
    return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

gsl::zstring<> Utilities::copyStringToNewChar(const std::string &str) {
  auto result = new char[str.length() + 1];
  strcpy(result, str.c_str());
  return result;
}

int8_t Utilities::compareInt(int a, int b) {
  if (a == b) {
    return 0;
  }
  if (a < b) {
    return -1;
  }
  return 1;
}

int8_t Utilities::compareDouble(double a, double b) {
  if (a == b) {
    return 0;
  }
  if (a < b) {
    return -1;
  }
  return 1;
}

int8_t Utilities::compareCurrency(Currency &a, Currency &b) {
  if (a == b) {
    return 0;
  }
  if (a < b) {
    return -1;
  }
  return 1;
}

int8_t Utilities::compareString(std::string a, std::string b) {
  auto cmpResult = a.compare(b);
  if (cmpResult < 0) {
    return -1;
  } else if (cmpResult > 0) {
    return 1;
  }
  return 0;
}

int8_t Utilities::compareDateTime(const DateTime &a, const DateTime &b) {
  if (a == b) {
    return 0;
  }
  if (a < b) {
    return -1;
  }
  return 1;
}

std::string Utilities::getRandomString(size_t length) {
  auto randChar = []() -> char {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randChar);
  return str;
}
