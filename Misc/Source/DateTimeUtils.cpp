//
// Created by Petr Flajsingr on 2019-02-22.
//

#include <DateTimeUtils.h>
#include <Utilities.h>

DateTime::DateTime(std::string_view str) {
  fromString(str);
}
void DateTime::fromString(std::string_view value) {
  std::stringstream ss;
  ss << value;
  ss >> *this;
}
std::string DateTime::toString() const {
  std::stringstream ss;
  ss << *this;
  return ss.str();
}
int32_t DateTime::getYear() const {
  return year;
}
void DateTime::setYear(int32_t year) {
  DateTime::year = year;
}
uint8_t DateTime::getMonth() const {
  return month;
}
void DateTime::setMonth(uint8_t month) {
  Expects(0 < month && month <= 12);
  DateTime::month = month;
}
uint8_t DateTime::getDay() const {
  return day;
}
void DateTime::setDay(uint8_t day) {
  //  TODO
  Expects(0 < day && day <= 31);
  DateTime::day = day;
}
uint8_t DateTime::getHour() const {
  return hour;
}
void DateTime::setHour(uint8_t hour) {
  Expects(hour <= 24);
  DateTime::hour = hour;
}
uint8_t DateTime::getMinute() const {
  return minute;
}
void DateTime::setMinute(uint8_t minute) {
  Expects(minute <= 59);
  DateTime::minute = minute;
}
uint8_t DateTime::getSecond() const {
  return second;
}
void DateTime::setSecond(uint8_t second) {
  Expects(second <= 59);
  DateTime::second = second;
}
uint16_t DateTime::getMillisecond() const {
  return millisecond;
}
void DateTime::setMillisecond(uint16_t millisecond) {
  Expects(millisecond <= 999);
  DateTime::millisecond = millisecond;
}
bool DateTime::operator==(const DateTime &rhs) const {
  return year == rhs.year &&
      month == rhs.month &&
      day == rhs.day &&
      hour == rhs.hour &&
      minute == rhs.minute &&
      second == rhs.second &&
      millisecond == rhs.millisecond;
}
bool DateTime::operator!=(const DateTime &rhs) const {
  return !(rhs == *this);
}
bool DateTime::operator<(const DateTime &rhs) const {
  if (year < rhs.year)
    return true;
  if (rhs.year < year)
    return false;
  if (month < rhs.month)
    return true;
  if (rhs.month < month)
    return false;
  if (day < rhs.day)
    return true;
  if (rhs.day < day)
    return false;
  if (hour < rhs.hour)
    return true;
  if (rhs.hour < hour)
    return false;
  if (minute < rhs.minute)
    return true;
  if (rhs.minute < minute)
    return false;
  if (second < rhs.second)
    return true;
  if (rhs.second < second)
    return false;
  return millisecond < rhs.millisecond;
}
bool DateTime::operator>(const DateTime &rhs) const {
  return rhs < *this;
}
bool DateTime::operator<=(const DateTime &rhs) const {
  return !(rhs < *this);
}
bool DateTime::operator>=(const DateTime &rhs) const {
  return !(*this < rhs);
}
std::ostream &operator<<(std::ostream &os, const DateTime &dateTime) {
  os << dateTime.year << "-" << (int) dateTime.month << "-"
     << (int) dateTime.day << " "
     << (int) dateTime.hour << "-" << (int) dateTime.minute << "-"
     << (int) dateTime.second
     << " "
     << (int) dateTime.millisecond;
  return os;
}
std::istream &operator>>(std::istream &is, DateTime &dateTime) {
  std::string temp;

  is >> temp;
  auto dateVals = Utilities::splitStringByDelimiter(temp, "-");
  dateTime.setYear(Utilities::stringToInt(dateVals[0]));
  dateTime.setMonth(Utilities::stringToInt(dateVals[1]));
  dateTime.setDay(Utilities::stringToInt(dateVals[2]));

  is >> temp;
  auto timeVals = Utilities::splitStringByDelimiter(temp, "-");
  dateTime.setHour(Utilities::stringToInt(timeVals[0]));
  dateTime.setMinute(Utilities::stringToInt(timeVals[1]));
  dateTime.setSecond(Utilities::stringToInt(timeVals[2]));
  dateTime.setMillisecond(Utilities::stringToInt(timeVals[3]));

  return is;
}
