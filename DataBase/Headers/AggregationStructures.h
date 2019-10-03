//
// Created by Petr Flajsingr on 2019-03-21.
//

#ifndef PROJECT_AGGREGATIONSTRUCTURES_H
#define PROJECT_AGGREGATIONSTRUCTURES_H

#include <Types.h>
#include <BaseField.h>
#include <gsl/gsl>

namespace DataBase {

struct BaseAgr {
  std::pair<gsl::index, gsl::index> fieldIndex;
  DataSets::BaseField *field;
  explicit BaseAgr(DataSets::BaseField *field);
};

struct Unique : public BaseAgr {
  DataContainer lastVal;
  explicit Unique(DataSets::BaseField *field);
  /**
   *
   * @param newVal
   * @return true if newVal doesn't equal lastVal, false otherwise
   */
  bool check(const DataContainer &newVal);
};

struct Sum : public BaseAgr {
  DataContainer sum;
  explicit Sum(DataSets::BaseField *field);
  void accumulate(const DataContainer &val);
  void reset();
};

struct Min : public BaseAgr {
  DataContainer min;
  explicit Min(DataSets::BaseField *field);
  void check();
  void reset();
};

struct Max : public BaseAgr {
  DataContainer max;
  explicit Max(DataSets::BaseField *field);
  void check();
  void reset();
};

struct Count : public BaseAgr {
  uint64_t count = 0;
  explicit Count(DataSets::BaseField *field);
  void add();
  void reset();
};

#pragma clang diagnostic push
#pragma ide diagnostic ignored "HidingNonVirtualFunction"
struct Avg : Sum, Count {
  explicit Avg(DataSets::BaseField *field);
  void accumulate(const DataContainer &val);
  void reset();
  DataContainer getResult();
};

#pragma clang diagnostic pop
}

#endif //PROJECT_AGGREGATIONSTRUCTURES_H
