//
// Created by Petr Flajsingr on 24/08/2018.
//

#ifndef CSV_READER_UTILITIES_H
#define CSV_READER_UTILITIES_H


#include <string>
#include <vector>
#include "../Types.h"

class Utilities {
public:
    /**
     * Rozdělení stringu pomocí určeného delimiteru.
     * Delimiter není v rozdělených řetězcích obsažen.
     * @param str string pro rozdělení
     * @param delimiter rozdělovač
     * @return vector částí
     */
    static std::vector<std::string> SplitStringByDelimiter(std::string str,
                                                           std::string delimiter);

    static int StringToInt(const std::string& str);

    static double StringToDouble(const std::string& str);

    static bool isInteger(const std::string& value);

    static bool isDouble(const std::string& value);

    static ValueType getType(const std::string& value);
};


#endif //CSV_READER_UTILITIES_H
