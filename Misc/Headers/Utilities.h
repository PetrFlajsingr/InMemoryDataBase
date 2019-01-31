//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef MISC_HEADERS_UTILITIES_H_
#define MISC_HEADERS_UTILITIES_H_

#include <string>
#include <vector>
#include "Types.h"
#include "Exceptions.h"
class DateTime;

/**
 * Obecne metody s ruznym vyuzitim
 */
namespace Utilities {
  /**
   * Rozdeleni stringu pomoci urceneho delimiteru.
   * Delimiter neni v rozdelenych retezcich obsazen.
   * @param str string pro rozdeleni
   * @param delimiter rozdelovac
   * @return vector casti
   */
  std::vector<std::string> splitStringByDelimiter(std::string str,
                                                  std::string delimiter);

  /**
   * Rozdeleni stringu pomoci urceneho delimiteru.
   * Delimiter neni v rozdelenych retezcich obsazen.
   * @param str string pro rozdeleni
   * @param delimiter rozdelovac
   * @return vector casti
   */
  std::vector<std::string> splitStringByDelimiterReserve(std::string str,
                                                         std::string delimiter,
                                                         int reserve);

  /**
   * Prevod string do int.
   *
   * Nedefinovane pro neplatne vstupy.
   * @param str string ve tvaru integer
   * @return
   */
  int stringToInt(const std::string &str);

  /**
   * Prevod string do double.
   *
   * Nedefinovane pro neplatne vstupy
   * @param str string ve tvaru double
   * @return
   */
  double stringToDouble(const std::string &str);

  /**
   * Kontrola jestli je string integer za pomoci regex
   * @param value
   * @return
   */
  bool isInteger(const std::string &value);

  /**
   * Kontrola jestli je string double za pomoci regex
   * @param value
   * @return
   */
  bool isDouble(const std::string &value);

  /**
   * Ziskani typu hodnoty za pomoci regex
   * @param value
   * @return
   */
  ValueType getType(const std::string &value);

  /**
   * Kontrola, jestli string konci jinym stringem
   * @param value Kontrolovany string
   * @param ending Hledany konec
   * @return true pokud value konci ending, jinak false
   */
  bool endsWith(std::string const &value, std::string const &ending);

  /**
   * Zkopirovani std::string do nove alokovane char*.
   * Vyuziva strdup()
   * @param str string pro zkopirovani
   * @return nove alokovany char* obsahujici str.data()
   */
  char *copyStringToNewChar(const std::string &str);

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
  int8_t compareCurrency(Currency &a, Currency &b);

  /**
   * Srovnani string hodnot
   * @param a
   * @param b
   * @return -1 pokud a < b, 0 pokud a == b, 1 pokud a > b
   */
  int8_t compareString(std::string a, std::string b);

  /**
   * Srovnani DateTime hodnot
   * @param a
   * @param b
   * @return -1 pokud a < b, 0 pokud a == b, 1 pokud a > b
   */
  int8_t compareDateTime(const DateTime &a, const DateTime &b);

std::string getRandomString(size_t length);
}  // namespace Utilities

#endif //  MISC_HEADERS_UTILITIES_H_
