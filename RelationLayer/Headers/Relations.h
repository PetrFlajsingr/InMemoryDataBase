//
// Created by Petr Flajsingr on 2019-01-29.
//

#ifndef CSV_READER_BASERELATION_H
#define CSV_READER_BASERELATION_H

#include <vector>
#include <Types.h>

namespace RelationLayer {

struct RelationContainer {
  DataContainer value_FirstDataSet;
  DataContainer value_SecondDataSet;
};

class BaseRelation {
 protected:
  std::vector<RelationContainer> relations;
 public:

};

class OneToOneRelation : public BaseRelation {

};

class OneToNRelation : public BaseRelation {

};

class NToNRelation : public BaseRelation {

};

}  // namespace RelationLayer

#endif //CSV_READER_BASERELATION_H
