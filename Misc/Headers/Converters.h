//
// Created by Petr Flajsingr on 2019-03-19.
//

#ifndef PROJECT_CONVERTERS_H
#define PROJECT_CONVERTERS_H

#include <string>
#include <boost/locale.hpp>

template<typename T, typename U>
class Converter {
 public:
  virtual U convert(const T &value) const = 0;
  virtual T convertBack(const U &value) const = 0;
};

class IntegerStringConverter : public Converter<int, std::string> {
 public:
  std::string convert(const int &value) const override;
  int convertBack(const std::string &value) const override;
};

class StringIntegerConverter : public Converter<std::string, int> {
 public:
  int convert(const std::string &value) const override;
  std::string convertBack(const int &value) const override;
};

class DoubleStringConverter : public Converter<double, std::string> {
 public:
  std::string convert(const double &value) const override;
  double convertBack(const std::string &value) const override;
};

class StringDoubleConverter : public Converter<std::string, double> {
 public:
  double convert(const std::string &value) const override;
  std::string convertBack(const double &value) const override;
};

class StringSplitConverter : public Converter<std::string,
                                              std::vector<std::string>> {
 public:
  explicit StringSplitConverter(const std::string &delimiter);
  std::vector<std::string> convert(const std::string &value) const override;
  std::string convertBack(const std::vector<std::string> &value) const override;
 private:
  std::string delimiter;
};

class CharSetConverter : public Converter<std::string, std::string> {
 public:
  enum class CharSet {
    Latin1, Latin2, CP1250, CP1252, Utf16, ANSI
  };

  explicit CharSetConverter(CharSet charsetIn);

  std::string convert(const std::string &value) const override;
  std::string convertBack(const std::string &value) const override;

 private:
  CharSet charsetIn;
};

#endif //PROJECT_CONVERTERS_H
