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
