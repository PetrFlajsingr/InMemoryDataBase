//
// Created by Petr Flajsingr on 2019-03-19.
//

#ifndef PROJECT_CONVERTERS_H
#define PROJECT_CONVERTERS_H

#include <Types.h>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/locale.hpp>
#include <string>
#include <vector>

enum class CharSet { Latin1, Latin2, CP1250, CP1252, Utf16, ANSI };

/**
 * Convert T to U and vice versa.
 */
template <typename T, typename U> class Converter {
public:
  [[nodiscard]] virtual U convert(const T &value) const = 0;

  [[nodiscard]] virtual T convertBack(const U &value) const = 0;
  virtual ~Converter() = default;
};

class IntegerStringConverter : public Converter<int, std::string> {
public:
  [[nodiscard]] std::string convert(const int &value) const override;

  [[nodiscard]] int convertBack(const std::string &value) const override;
};

class StringIntegerConverter : public Converter<std::string, int> {
public:
  [[nodiscard]] int convert(const std::string &value) const override;

  [[nodiscard]] std::string convertBack(const int &value) const override;
};

class DoubleStringConverter : public Converter<double, std::string> {
public:
  [[nodiscard]] std::string convert(const double &value) const override;

  [[nodiscard]] double convertBack(const std::string &value) const override;
};

class StringDoubleConverter : public Converter<std::string, double> {
public:
  [[nodiscard]] double convert(const std::string &value) const override;

  [[nodiscard]] std::string convertBack(const double &value) const override;
};

class StringSplitConverter : public Converter<std::string, std::vector<std::string>> {
public:
  explicit StringSplitConverter(std::string delimiter);

  [[nodiscard]] std::vector<std::string> convert(const std::string &value) const override;

  [[nodiscard]] std::string convertBack(const std::vector<std::string> &value) const override;

private:
  std::string delimiter;
};

class ExcelDateTime2DateTimeConverter : public Converter<double, DateTime> {
public:
  explicit ExcelDateTime2DateTimeConverter(const boost::gregorian::date &customOrigin = boost::gregorian::date(1900, 1, 1));

  [[nodiscard]] DateTime convert(const double &value) const override;
  [[nodiscard]] double convertBack(const DateTime &value) const override;

protected:
  const boost::gregorian::date excelStartDate;

  static constexpr unsigned int hoursInDay = 24;
  static constexpr unsigned int minutesInHour = 60;
  static constexpr unsigned int secondsInMinute = 60;
  static constexpr unsigned int daySecondCount = hoursInDay * minutesInHour * secondsInMinute;

private:
  static constexpr unsigned int windowsExcelDateError = 2;
  DateTimeType type = DateTimeType::DateTime;
};

class MacExcelDateTime2DateTimeConverter : public ExcelDateTime2DateTimeConverter {
public:
  explicit MacExcelDateTime2DateTimeConverter(const boost::gregorian::date &customOrigin = boost::gregorian::date(1904, 1, 1));

  [[nodiscard]] DateTime convert(const double &value) const override;
  [[nodiscard]] double convertBack(const DateTime &value) const override;

private:
  DateTimeType type = DateTimeType::DateTime;
};

class CharSetConverter : public Converter<std::string, std::string> {
public:
  explicit CharSetConverter(CharSet charsetIn);

  [[nodiscard]] std::string convert(const std::string &value) const override;

  [[nodiscard]] std::string convertBack(const std::string &value) const override;

private:
  CharSet charsetIn;
};

xlnt::date doubleToDate(double value);
#endif // PROJECT_CONVERTERS_H
