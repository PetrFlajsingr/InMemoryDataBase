//
// Created by Petr Flajsingr on 24/08/2018.
//

#include <Types.h>
#include <Utilities.h>
#include <thread>
#include <utf8cpp/utf8.h>

std::vector<std::string> Utilities::splitStringByDelimiter(std::string_view str, std::string_view delimiter) {
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

std::vector<std::string> Utilities::splitStringByDelimiterReserve(std::string_view str, std::string_view delimiter,
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
  const auto result = strtod(std::string(str).c_str(), &p);
  if (*p) {
    return 0;
  } else {
    return result;
  }
}

double Utilities::stringToDouble(std::string_view str) {
  gsl::zstring<> p;
  const auto result = strtod(std::string(str).c_str(), &p);
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

bool Utilities::endsWith(std::string_view value, std::string_view ending) {
  if (ending.size() > value.size()) {
    return false;
  }
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

// TODO: epsilon
int8_t Utilities::compareDouble(double a, double b, double epsilon) {
  if (std::abs(a - b) < epsilon) {
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

int8_t Utilities::compareString(std::experimental::u8string_view a, std::experimental::u8string_view b) {
  auto cmpResult = a == b;
  if (cmpResult) {
    return 0;
  }
  if (a < b) {
    return -1;
  } else if (a > b) {
    return 1;
  }
  return 0;
}
int8_t Utilities::compareU8String(std::experimental::u8string_view a, std::experimental::u8string_view b) {
  auto begin1 = a.begin();
  const auto end1 = a.end();
  auto begin2 = b.begin();
  const auto end2 = b.end();
  const auto aLength = utf8::distance(begin1, end1);
  const auto bLength = utf8::distance(begin2, end2);
  while (begin1 < end1 && begin2 < end2) {
    const auto val1 = utf8::next(begin1, end1);
    const auto val2 = utf8::next(begin2, end2);
    if ((val1 == 160 && val2 == 32) || (val1 == 32 && val2 == 160)) {
      continue;
    } else if (val1 < val2) {
      return -1;
    } else if (val1 > val2) {
      return 1;
    }
  }
  if (aLength < bLength) {
    return -1;
  } else if (aLength > bLength) {
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
    const char charset[] = "0123456789"
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

template <class T> int8_t Utilities::compare(const T &a, const T &b) {
  if (a == b) {
    return 0;
  }
  if (a > b) {
    return 1;
  }
  return -1;
}
int Utilities::getCoreCount() {
  auto result = std::thread::hardware_concurrency();
  if (result == 0) {
    result = 1;
  }
  return result;
}
std::string Utilities::boolToString(bool value) {
  return value ? "true" : "false";
}

bool Utilities::isUtf8Accent(char c) {
  return std::find(utf8Accents.begin(), utf8Accents.end(), c) != utf8Accents.end();
}
std::string Utilities::rtrim(std::string str, const std::string &chars) {
  str.erase(str.find_last_not_of(chars) + 1);
  return str;
}
std::string Utilities::ltrim(std::string str, const std::string &chars) {
  str.erase(0, str.find_first_not_of(chars));
  return str;
}
std::string Utilities::trim(std::string str, const std::string &chars) {
  return ltrim(rtrim(str, chars), chars);
}
