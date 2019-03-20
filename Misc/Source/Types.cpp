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

const std::string DateTimeB::dateTimeDefFmt = "%d/%m/%Y %H:%M:%S";
const std::string DateTimeB::dateDefFmt = "%d/%m/%Y";
const std::string DateTimeB::timeDefFmt = "%H:%M:%S";

DateTimeB::DateTimeB(const boost::posix_time::ptime &ptime,
                     DateTimeType type)
    : type(type), ptime(ptime) {}

std::string DateTimeB::toString() const {
  switch (type) {
    case DateTimeType::Date: return toString(dateDefFmt);
    case DateTimeType::Time: return toString(timeDefFmt);
    case DateTimeType::DateTime: return toString(dateTimeDefFmt);
  }
}

std::string DateTimeB::toString(std::string_view fmt) const {
  boost::posix_time::time_facet *facet = new boost::posix_time::time_facet(std::string(fmt).c_str());
  std::ostringstream os;
  os.imbue(std::locale(os.getloc(), facet));
  os << ptime;
  return os.str();
}

DateTimeType DateTimeB::getType() const {
  return type;
}
DateTimeB DateTimeB::fromString(std::string_view str, DateTimeType type) {
  switch (type) {
    case DateTimeType::Date: return fromString(str, dateDefFmt);
    case DateTimeType::Time: return fromString(str, timeDefFmt);
    case DateTimeType::DateTime: return fromString(str, dateTimeDefFmt);
  }
}
DateTimeB DateTimeB::fromString(std::string_view str, std::string_view fmt) {
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
  return DateTimeB(t, type);
}
const boost::posix_time::ptime &DateTimeB::getTime() const {
  return ptime;
}

