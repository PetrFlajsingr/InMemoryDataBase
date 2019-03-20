//
// Created by Petr Flajsingr on 2019-02-01.
//

#include <Types.h>

std::string OperationToString(Operation op) {
  switch (op) {
    case Operation::Distinct:return "";
    case Operation::Sum:return "(Sum)";
    case Operation::Average:return "(Avg)";
  }
}

int compareDataContainers(const DataContainer &data1,
                          const DataContainer &data2,
                          ValueType valueType) {
  switch (valueType) {
    case ValueType::Integer:
      return Utilities::compareInt(data1._integer,
                                   data2._integer);
    case ValueType::Double:
      return Utilities::compareDouble(data1._double,
                                      data2._double);
    case ValueType::String:
      return Utilities::compareString(data1._string,
                                      data2._string);
    case ValueType::Currency:
      return Utilities::compareCurrency(*data1._currency,
                                        *data2._currency);
    case ValueType::DateTime:
      return Utilities::compareDateTime(*data1._dateTime,
                                        *data2._dateTime);
  }
  throw IllegalStateException("Internal error. compareDataContainers");
}
DataContainer &DataContainer::operator=(gsl::zstring<> val) {
  _string = val;
  return *this;
}
DataContainer &DataContainer::operator=(int val) {
  _integer = val;
  return *this;
}
DataContainer &DataContainer::operator=(double val) {
  _double = val;
  return *this;
}
DataContainer &DataContainer::operator=(const Currency &val) {
  if (_currency == nullptr) {
    _currency = new Currency();
  }
  *_currency = val;
  return *this;
}
DataContainer &DataContainer::operator=(const DateTime &val) {
  if (_dateTime == nullptr) {
    _dateTime = new DateTime();
  }
  *_dateTime = val;
  return *this;
}

const std::string DateTime::dateTimeDefFmt = "%d/%m/%Y %H:%M:%S";
const std::string DateTime::dateDefFmt = "%d/%m/%Y";
const std::string DateTime::timeDefFmt = "%H:%M:%S";

DateTime::DateTime() : type(DateTimeType::DateTime), ptime(boost::posix_time::second_clock::local_time()) {}
DateTime::DateTime(DateTimeType type) : type(type), ptime(boost::posix_time::second_clock::local_time()) {}

DateTime::DateTime(const boost::posix_time::ptime &ptime,
                     DateTimeType type)
    : type(type), ptime(ptime) {}

DateTime::DateTime(const DateTime &other)
    : type(other.type), ptime(other.ptime) {}

std::string DateTime::toString() const {
  switch (type) {
    case DateTimeType::Date: return toString(dateDefFmt);
    case DateTimeType::Time: return toString(timeDefFmt);
    case DateTimeType::DateTime: return toString(dateTimeDefFmt);
  }
}

std::string DateTime::toString(std::string_view fmt) const {
  boost::posix_time::time_facet *facet = new boost::posix_time::time_facet(std::string(fmt).c_str());
  std::ostringstream os;
  os.imbue(std::locale(os.getloc(), facet));
  os << ptime;
  return os.str();
}

DateTimeType DateTime::getType() const {
  return type;
}

DateTime::DateTime(std::string_view str, DateTimeType type) : type(type) {
  fromString(str);
}
DateTime::DateTime(std::string_view str, std::string_view fmt) {
  fromString(str, fmt);
}

const boost::posix_time::ptime &DateTime::getTime() const {
  return ptime;
}
bool DateTime::operator==(const DateTime &rhs) const {
  return type == rhs.type &&
      ptime == rhs.ptime;
}
bool DateTime::operator!=(const DateTime &rhs) const {
  return !(rhs == *this);
}
bool DateTime::operator<(const DateTime &rhs) const {
  return ptime < rhs.ptime;
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
std::ostream &operator<<(std::ostream &os, const DateTime &timeB) {
  os << timeB.toString();
  return os;
}
std::istream &operator>>(std::istream &is, DateTime &timeB) {
  throw NotImplementedException();
}
std::pair<DateTimeType, boost::posix_time::ptime> DateTime::innerFromString(std::string_view str,
                                                                            std::string_view fmt) {
  const std::locale loc = std::locale(std::locale::classic(),
                                      new boost::posix_time::time_input_facet(std::string(fmt)));
  auto is = std::istringstream(std::string(str));
  is.imbue(loc);
  boost::posix_time::ptime t;
  is >> t;
  DateTimeType type;
  if (fmt.find("%d") != std::string::npos) {
    if (fmt.find("%H") != std::string::npos) {
      type = DateTimeType::DateTime;
    } else {
      type = DateTimeType::Date;
    }
  } else {
    type = DateTimeType::Time;
  }
  return std::make_pair(type, t);
}

void DateTime::fromString(std::string_view str) {
  switch (type) {
    case DateTimeType::Date: fromString(str, dateDefFmt);
      break;
    case DateTimeType::Time: fromString(str, timeDefFmt);
      break;
    case DateTimeType::DateTime: fromString(str, dateTimeDefFmt);
      break;
  }
}
void DateTime::fromString(std::string_view str, std::string_view fmt) {
  auto[type, t] = innerFromString(str, fmt);
  this->ptime = t;
  this->type = type;
}

