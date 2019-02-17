//
// Created by Petr Flajsingr on 24/08/2018.
//

#include <Utilities.h>
#include <DateTimeUtils.h>

std::vector<std::string> Utilities::splitStringByDelimiter(std::string_view str,
                                                           std::string_view delimiter) {
  std::vector<std::string> result;
  size_t first = 0;

  while (first < str.size()) {
    const auto second = str.find_first_of(delimiter, first);

    if (first != second) {
      result.emplace_back(str.substr(first, second - first));
    }
    if (second == std::string_view::npos) {
      break;
    }
    first = second + 1;
  }

  return result;
}

std::vector<std::string> Utilities::splitStringByDelimiterReserve(std::string_view str,
                                                                  std::string_view delimiter,
                                                                  int reserve) {
  std::vector<std::string> result;
  result.reserve(reserve);
  size_t first = 0;

  while (first < str.size()) {
    const auto second = str.find_first_of(delimiter, first);

    if (first != second) {
      result.emplace_back(str.substr(first, second - first));
    }
    if (second == std::string_view::npos) {
      break;
    }
    first = second + 1;
  }

  return result;
}

int Utilities::stringToInt(std::string_view str) {
  gsl::zstring<> p;
  auto result = strtol(std::string(str).c_str(), &p, 10);
  if (*p) {
    return 0;
  } else {
    return result;
  }
}

double Utilities::stringToDouble(std::string_view str) {
  gsl::zstring<> p;
  auto result = strtod(std::string(str).c_str(), &p);
  if (*p) {
    return 0;
  } else {
    return result;
  }
}

bool Utilities::isInteger(std::string_view value) {
  std::regex integerRegex("(\\+|-)?[1-9][0-9]*");

  return std::regex_match(std::string(value), integerRegex);
}

bool Utilities::isDouble(std::string_view value) {
  std::regex doubleRegex("[+-]?([0-9]*[.])?[0-9]+");

  return std::regex_match(std::string(value), doubleRegex);
}

ValueType Utilities::getType(std::string_view value) {
  if (isInteger(value)) {
    return ValueType::Integer;
  } else if (isDouble(value)) {
    return ValueType::Double;
  } else {
    return ValueType::String;
  }
}

bool Utilities::endsWith(std::string_view value, std::string_view ending) {
  if (ending.size() > value.size())
    return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

gsl::zstring<> Utilities::copyStringToNewChar(std::string_view str) {
  auto result = new char[str.length() + 1];
  strcpy(result, std::string(str).c_str());
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

int8_t Utilities::compareCurrency(const Currency &a, const Currency &b) {
  if (a == b) {
    return 0;
  }
  if (a < b) {
    return -1;
  }
  return 1;
}

int8_t Utilities::compareString(std::string_view a, std::string_view b) {
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc30-c"
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
#pragma clang diagnostic pop

std::string Utilities::toLower(const std::string &str) {
  std::string result;
  std::transform(str.begin(), str.end(), std::back_inserter(result), ::tolower);
  return result;
}

template<class T>
int8_t Utilities::compare(const T &a, const T &b) {
  if (a == b) {
    return 0;
  }
  if (a > b) {
    return 1;
  }
  return -1;
}
