//
// Created by Petr Flajsingr on 2019-03-19.
//

#include <Exceptions.h>
#include "Converters.h"
#include <Utilities.h>

std::string CharSetConverter::convert(const std::string &value) const {
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
std::string CharSetConverter::convertBack(const std::string &value) const {
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

CharSetConverter::CharSetConverter(CharSet charsetIn)
    : charsetIn(charsetIn) {}
std::string IntegerStringConverter::convert(const int &value) const {
  return std::to_string(value);
}
int IntegerStringConverter::convertBack(const std::string &value) const {
  return Utilities::stringToInt(value);
}

int StringIntegerConverter::convert(const std::string &value) const {
  return Utilities::stringToInt(value);
}
std::string StringIntegerConverter::convertBack(const int &value) const {
  return std::to_string(value);
}

std::string DoubleStringConverter::convert(const double &value) const {
  return std::to_string(value);
}
double DoubleStringConverter::convertBack(const std::string &value) const {
  return Utilities::stringToDouble(value);
}

double StringDoubleConverter::convert(const std::string &value) const {
  return Utilities::stringToDouble(value);
}
std::string StringDoubleConverter::convertBack(const double &value) const {
  return std::to_string(value);
}

StringSplitConverter::StringSplitConverter(const std::string &delimiter)
    : delimiter(delimiter) {}
std::vector<std::string> StringSplitConverter::convert(const std::string &value) const {
  return Utilities::splitStringByDelimiter(value, delimiter);
}
std::string StringSplitConverter::convertBack(const std::vector<std::string> &value) const {
  std::string result;
  for (gsl::index i = 0; i < value.size() - 1; ++i) {
    result.append(value[i]);
    result.append(delimiter);
  }
  result.append(value.back());
  return result;
}

boost::posix_time::ptime ExcelDateTime2DateTimeConverter::convert(const double &value) const {
  using namespace boost::gregorian;
  using namespace boost::posix_time;
  auto finDate = excelStartDate + date_duration(gsl::narrow_cast<int>(value) - 2);
  auto hourF = (value - gsl::narrow_cast<int>(value)) * 24;
  auto h = gsl::narrow_cast<uint8_t>(hourF);
  auto minF = (hourF - h) * 60;
  auto m = gsl::narrow_cast<uint8_t>(minF);
  auto secF = (minF - m) * 60;
  auto s = gsl::narrow_cast<uint8_t>(round(secF));
  return ptime(finDate, hours(h) + minutes(m) + seconds(s));
}
double ExcelDateTime2DateTimeConverter::convertBack(const boost::posix_time::ptime &value) const {
  constexpr double daySecondCount = 24 * 60 * 60;
  auto wholePart = (value.date() - excelStartDate).days() + 2;
  auto fractPart = value.time_of_day().total_seconds() / daySecondCount;
  return wholePart + fractPart;
}
