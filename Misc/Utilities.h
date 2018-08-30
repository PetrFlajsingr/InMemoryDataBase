//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef CSV_READER_UTILITIES_H
#define CSV_READER_UTILITIES_H


#include <string>
#include <vector>
#include "Types.h"

/**
 * Obecne metody pro
 */
class Utilities {
public:
    /**
     * Rozdeleni stringu pomoci urceneho delimiteru.
     * Delimiter neni v rozdelenych retezcich obsazen.
     * @param str string pro rozdeleni
     * @param delimiter rozdelovac
     * @return vector casti
     */
    static std::vector<std::string> splitStringByDelimiter(std::string str,
                                                           std::string delimiter);

    /**
     * Prevod string do int.
     * @param str string ve tvaru integer
     * @return
     */
    static int stringToInt(const std::string &str);

    /**
     * Prevod string do double
     * @param str string ve tvaru double
     * @return
     */
    static double stringToDouble(const std::string &str);

    /**
     * Kontrola jestli je string integer za pomoci regex
     * @param value
     * @return
     */
    static bool isInteger(const std::string& value);

    /**
     * Kontrola jestli je string double za pomoci regex
     * @param value
     * @return
     */
    static bool isDouble(const std::string& value);

    /**
     * Ziskani typu hodnoty za pomoci regex
     * @param value
     * @return
     */
    static ValueType getType(const std::string& value);
};


#endif //CSV_READER_UTILITIES_H
