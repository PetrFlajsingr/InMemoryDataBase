//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef MISC_HEADERS_UTILITIES_H_
#define MISC_HEADERS_UTILITIES_H_

#include "Exceptions.h"
#include <decimal.h>
#include <dtoa_milo.h>
#include <gsl/gsl>
#include <regex>
#include <sstream>
#include <string>
#include <experimental/string_view>
#include <vector>
#include <iostream>
#include <meta.h>

using Currency = dec::decimal<2>;
class DateTime;

namespace Utilities {

std::string defaultForEmpty(const std::string &value, const std::string &def = "");

std::string replaceAll(
    const std::string & str ,   // where to work
    const std::string & find ,  // substitute 'find'
    const std::string & replace //      by 'replace'
);

/**
 * Rozdeleni stringu pomoci urceneho delimiteru.
 * Delimiter neni v rozdelenych retezcich obsazen.
 * @param str string pro rozdeleni
 * @param delimiter rozdelovac
 * @return vector casti
 */
std::vector<std::string> splitStringByDelimiter(std::string_view str, std::string_view delimiter);
/**
 * Rozdeleni stringu pomoci urceneho delimiteru.
 * Delimiter neni v rozdelenych retezcich obsazen.
 * @param str string pro rozdeleni
 * @param delimiter rozdelovac
 * @return vector casti
 */
std::vector<std::string> splitStringByDelimiterReserve(std::string_view str, std::string_view delimiter, int reserve);
/**
 * Prevod string do int.
 *
 * Nedefinovane pro neplatne vstupy.
 * @param str string ve tvaru integer
 * @return
 */
int stringToInt(std::string_view str);
/**
 * Prevod string do double.
 *
 * Nedefinovane pro neplatne vstupy
 * @param str string ve tvaru double
 * @return
 */
double stringToDouble(std::string_view str);
/**
 * Kontrola jestli je string integer za pomoci regex
 * @param value
 * @return
 */
bool isInteger(std::string_view value);
/**
 * Kontrola jestli je string double za pomoci regex
 * @param value
 * @return
 */
bool isDouble(std::string_view value);
/**
 * Kontrola, jestli string konci jinym stringem
 * @param value Kontrolovany string
 * @param ending Hledany konec
 * @return true pokud value konci ending, jinak false
 */
bool endsWith(std::string_view value, std::string_view ending);
/**
 * Zkopirovani std::string do nove alokovane char*.
 * Vyuziva strdup()
 * @param str string pro zkopirovani
 * @return nove alokovany char* obsahujici str.data()
 */
gsl::zstring<> copyStringToNewChar(std::string_view str);
template <typename T> int8_t compare(const T &a, const T &b);
/**
 * Srovnani integer hodnot
 * @param a
 * @param b
 * @return -1 pokud a < b, 0 pokud a == b, 1 pokud a > b
 */
int8_t compareInt(int a, int b);
/**
 * Srovnani double hodnot
 * @param a
 * @param b
 * @return -1 pokud a < b, 0 pokud a == b, 1 pokud a > b
 */
int8_t compareDouble(double a, double b, double epsilon = 1e-7);
/**
 * Srovnani Currency hodnot
 * @param a
 * @param b
 * @return -1 pokud a < b, 0 pokud a == b, 1 pokud a > b
 */
int8_t compareCurrency(const Currency &a, const Currency &b);
/**
 * Srovnani string hodnot
 * @param a
 * @param b
 * @return -1 pokud a < b, 0 pokud a == b, 1 pokud a > b
 */
int8_t compareString(std::string_view a, std::string_view b);
/**
 * Srovnani string hodnot
 * @param a
 * @param b
 * @return -1 pokud a < b, 0 pokud a == b, 1 pokud a > b
 */
int8_t compareString(std::experimental::u8string_view a, std::experimental::u8string_view b);
int8_t compareU8String(std::experimental::u8string_view a, std::experimental::u8string_view b);
/**
 * Srovnani DateTime hodnot
 * @param a
 * @param b
 * @return -1 pokud a < b, 0 pokud a == b, 1 pokud a > b
 */
int8_t compareDateTime(const DateTime &a, const DateTime &b);

/**
 * Generates a string with given length. The string contains alphanumeric symbols
 * @param length desired length of the string
 * @return
 */
std::string getRandomString(size_t length);

std::string toLower(const std::string &str);

template <int maxDigit = 64> std::string doubleToString(double value);

std::string boolToString(bool value);

int getCoreCount();

namespace {
const std::array<char, 64> utf8Accents = {
    gsl::narrow_cast<char>(0x80), gsl::narrow_cast<char>(0x81), gsl::narrow_cast<char>(0x82),
    gsl::narrow_cast<char>(0x83), gsl::narrow_cast<char>(0x84), gsl::narrow_cast<char>(0x85),
    gsl::narrow_cast<char>(0x86), gsl::narrow_cast<char>(0x87), gsl::narrow_cast<char>(0x88),
    gsl::narrow_cast<char>(0x89), gsl::narrow_cast<char>(0x8a), gsl::narrow_cast<char>(0x8b),
    gsl::narrow_cast<char>(0x8c), gsl::narrow_cast<char>(0x8d), gsl::narrow_cast<char>(0x8e),
    gsl::narrow_cast<char>(0x8f), gsl::narrow_cast<char>(0x90), gsl::narrow_cast<char>(0x91),
    gsl::narrow_cast<char>(0x92), gsl::narrow_cast<char>(0x93), gsl::narrow_cast<char>(0x94),
    gsl::narrow_cast<char>(0x95), gsl::narrow_cast<char>(0x96), gsl::narrow_cast<char>(0x98),
    gsl::narrow_cast<char>(0x99), gsl::narrow_cast<char>(0x9a), gsl::narrow_cast<char>(0x9b),
    gsl::narrow_cast<char>(0x9c), gsl::narrow_cast<char>(0x9d), gsl::narrow_cast<char>(0x9e),
    gsl::narrow_cast<char>(0x9f), gsl::narrow_cast<char>(0xa0), gsl::narrow_cast<char>(0xa1),
    gsl::narrow_cast<char>(0xa2), gsl::narrow_cast<char>(0xa3), gsl::narrow_cast<char>(0xa4),
    gsl::narrow_cast<char>(0xa5), gsl::narrow_cast<char>(0xa6), gsl::narrow_cast<char>(0xa7),
    gsl::narrow_cast<char>(0xa8), gsl::narrow_cast<char>(0xa9), gsl::narrow_cast<char>(0xaa),
    gsl::narrow_cast<char>(0xab), gsl::narrow_cast<char>(0xac), gsl::narrow_cast<char>(0xad),
    gsl::narrow_cast<char>(0xae), gsl::narrow_cast<char>(0xaf), gsl::narrow_cast<char>(0xb0),
    gsl::narrow_cast<char>(0xb1), gsl::narrow_cast<char>(0xb2), gsl::narrow_cast<char>(0xb3),
    gsl::narrow_cast<char>(0xb4), gsl::narrow_cast<char>(0xb5), gsl::narrow_cast<char>(0xb6),
    gsl::narrow_cast<char>(0xb8), gsl::narrow_cast<char>(0xb9), gsl::narrow_cast<char>(0xba),
    gsl::narrow_cast<char>(0xbb), gsl::narrow_cast<char>(0xbc), gsl::narrow_cast<char>(0xbd),
    gsl::narrow_cast<char>(0xbe), gsl::narrow_cast<char>(0xbf), gsl::narrow_cast<char>(0xc3),
    gsl::narrow_cast<char>(0xc5)
};
}

bool isUtf8Accent(char c);

namespace {
template <int indentLevel>
void printIndent() {
  std::cout << std::string(indentLevel * 2, ' ');
}
}
template <typename T, int indentLevel = 0>
void print(const T &value) {
  if constexpr (is_container<T>::value) {
    printIndent<indentLevel>();
    std::cout << '{' << std::endl;
    for (const auto &val: value) {
      print<std::decay_t<decltype(val)>, indentLevel + 1>(val);
    }
    printIndent<indentLevel>();
    std::cout << '}' << std::endl;
  } else {
    printIndent<indentLevel>();
    std::cout << value << std::endl;
  }
}

template <template <class> typename Container, typename T>
std::vector<T> getDuplicates(const Container<T> &container) {
  std::vector<T> result;
  for (const auto &value : container) {
    int count = -1;
    for (const auto &val : container) {
      if (value == val) {
        ++count;
      }
    }
    for (int i = 0; i < count; ++i) {
      result.emplace_back(value);
    }
  }
  return result;
}

std::string ltrim(std::string str, const std::string& chars = "\t\n\v\f\r ");

std::string rtrim(std::string str, const std::string& chars = "\t\n\v\f\r ");

std::string trim(std::string str, const std::string& chars = "\t\n\v\f\r ");
} // namespace Utilities

template <int maxDigit> std::string Utilities::doubleToString(double value) {
  static char buffer[maxDigit];
  dtoa_milo(value, buffer);
  return std::string(buffer);
}

#endif //  MISC_HEADERS_UTILITIES_H_
