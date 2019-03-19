//
// Created by Petr Flajsingr on 2019-03-19.
//

#include <Exceptions.h>
#include "Converters.h"

std::string CharSetConverter::convert(const std::string &value) {
  switch (charsetIn) {
    case CharSet::Latin1:
      return boost::locale::conv::to_utf<char>(value,
                                               "Latin1");
    case CharSet::Latin2:
      return boost::locale::conv::to_utf<char>(value,
                                               "Latin2");
    case CharSet::CP1250:
      return boost::locale::conv::to_utf<char>(value,
                                               "CP1250");
    case CharSet::CP1252:
      return boost::locale::conv::to_utf<char>(value,
                                               "CP1252");
    case CharSet::Utf16:
      return boost::locale::conv::to_utf<char>(value,
                                               "Utf16");
    case CharSet::ANSI:
      return boost::locale::conv::to_utf<char>(value,
                                               "ANSI");
  }
}
std::string CharSetConverter::convertBack(const std::string &value) {
  switch (charsetIn) {
    case CharSet::Latin1:
      return boost::locale::conv::from_utf<char>(value,
                                                 "Latin1");
    case CharSet::Latin2:
      return boost::locale::conv::from_utf<char>(value,
                                                 "Latin2");
    case CharSet::CP1250:
      return boost::locale::conv::from_utf<char>(value,
                                                 "CP1250");
    case CharSet::CP1252:
      return boost::locale::conv::from_utf<char>(value,
                                                 "CP1252");
    case CharSet::Utf16:
      return boost::locale::conv::from_utf<char>(value,
                                                 "Utf16");
    case CharSet::ANSI:
      return boost::locale::conv::from_utf<char>(value,
                                                 "ANSI");
  }
}

CharSetConverter::CharSetConverter(CharSetConverter::CharSet charsetIn)
    : charsetIn(charsetIn) {}
std::string IntegerStringConverter::convert(const int &value) {
  return std::to_string(value);
}
int IntegerStringConverter::convertBack(const std::string &value) {
  return Utilities::stringToInt(value);
}

int StringIntegerConverter::convert(const std::string &value) {
  return Utilities::stringToInt(value);
}
std::string StringIntegerConverter::convertBack(const int &value) {
  return std::to_string(value);
}

std::string DoubleStringConverter::convert(const double &value) {
  return std::to_string(value);
}
double DoubleStringConverter::convertBack(const std::string &value) {
  return Utilities::stringToDouble(value);
}

double StringDoubleConverter::convert(const std::string &value) {
  return Utilities::stringToDouble(value);
}
std::string StringDoubleConverter::convertBack(const double &value) {
  return std::to_string(value);
}

StringSplitConverter::StringSplitConverter(const std::string &delimiter)
    : delimiter(delimiter) {}
std::vector<std::string> StringSplitConverter::convert(const std::string &value) {
  return Utilities::splitStringByDelimiter(value, delimiter);
}
std::string StringSplitConverter::convertBack(const std::vector<std::string> &value) {
  std::string result;
  for (gsl::index i = 0; i < value.size() - 1; ++i) {
    result.append(val)
    result.append(delimiter)
  }
  result.append(value.back());
  return result;
}

