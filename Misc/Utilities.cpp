//
// Created by Petr Flajsingr on 24/08/2018.
//

#include <sstream>
#include <regex>
#include <utility>
#include "Utilities.h"

std::vector<std::string> Utilities::SplitStringByDelimiter(std::string str,
                                                           std::string delimiter) {
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

int Utilities::StringToInt(const std::string &str) {
    std::stringstream stringstream(str);

    int result;

    stringstream >> result;

    return result;
}

double Utilities::StringToDouble(const std::string &str) {
    std::stringstream stringstream(str);

    double result;

    stringstream >> result;

    return result;
}

bool Utilities::isInteger(const std::string& value) {
    std::regex integerRegex("(\\+|-)?[[:digit:]]+");

    return std::regex_match(value, integerRegex);
}

bool Utilities::isDouble(const std::string &value) {
    std::regex doubleRegex("[+-]?([:digit:]*[.])?[:digit:]+");

    return std::regex_match(value, doubleRegex);
}

ValueType Utilities::getType(const std::string &value) {
    if(isInteger(value)){
        return INTEGER_VAL;
    } else if(isDouble(value)) {
        return DOUBLE_VAL;
    } else {
        return STRING_VAL;
    }
}
