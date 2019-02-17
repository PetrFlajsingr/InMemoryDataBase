//
// Created by Petr Flajsingr on 2019-02-01.
//

#include <Types.h>
#include <Utilities.h>

std::string OperationToString(Operation op) {
  switch (op) {
    case Operation::Distinct:return "";
    case Operation::Sum:return "(Sum)";
    case Operation::Average:return "(Avg)";
  }
}
int compareDataContainers(DataContainer &data1,
                          DataContainer &data2,
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
