//
// Created by Petr Flajsingr on 24/08/2018.
//

#include "Utilities.h"

std::vector<std::string> Utilities::SplitStringByDelimiter(std::string str,
                                                           std::string delimiter) {
    std::vector<std::string> result;
    std::string toSplit = std::string(str);

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
