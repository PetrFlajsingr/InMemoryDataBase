//
// Created by Petr Flajsingr on 2019-01-29.
//

#ifndef CSV_READER_BASERELATION_H
#define CSV_READER_BASERELATION_H

#include <vector>
#include <Types.h>

namespace RelationLayer {

struct RelationContainer {
  void *data_FirstDataSet;
  void *data_SecondDataSet;
};

class BaseRelation {
 protected:
  std::vector<RelationContainer> relations;

 public:
  virtual void addRelation(void *dataFirstDataSet, void *dataSecondDataSet) = 0;

  virtual void removeRelation(void *dataFirstDataSet, void *dataSecondDataSet) = 0;

  virtual const std::vector<RelationContainer> &findRelations(void *pointerToData) = 0;
};

class OneToOneRelation : public BaseRelation {
 public:
  void addRelation(void *dataFirstDataSet, void *dataSecondDataSet) override;

  void removeRelation(void *dataFirstDataSet, void *dataSecondDataSet) override;

  const std::vector<RelationContainer> &findRelations(void *pointerToData) override;

};

class OneToNRelation : public BaseRelation {
 public:
  void addRelation(void *dataFirstDataSet, void *dataSecondDataSet) override;

  void removeRelation(void *dataFirstDataSet, void *dataSecondDataSet) override;

  const std::vector<RelationContainer> &findRelations(void *pointerToData) override;

};

class NToNRelation : public BaseRelation {
 public:
  void addRelation(void *dataFirstDataSet, void *dataSecondDataSet) override;

  void removeRelation(void *dataFirstDataSet, void *dataSecondDataSet) override;

  const std::vector<RelationContainer> &findRelations(void *pointerToData) override;

};

}  // namespace RelationLayer

#endif //CSV_READER_BASERELATION_H
