//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef MISC_HEADERS_UTILITIES_H_
#define MISC_HEADERS_UTILITIES_H_

#include <gsl/gsl>
#include <sstream>
#include <regex>
#include <string>
#include <vector>
#include "Exceptions.h"
#include <decimal.h>

typedef dec::decimal<2> Currency;
class DateTime;

namespace Utilities {
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
std::vector<std::string> splitStringByDelimiterReserve(std::string_view str,
                                                       std::string_view delimiter,
                                                       int reserve);
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
template<typename T>
int8_t compare(const T &a, const T &b);
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
int8_t compareDouble(double a, double b);
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

int getCoreCount();
}  // namespace Utilities

#endif //  MISC_HEADERS_UTILITIES_H_
