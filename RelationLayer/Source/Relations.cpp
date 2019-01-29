//
// Created by Petr Flajsingr on 2019-01-29.
//

#include <Relations.h>
#include <Exceptions.h>

RelationLayer::RelationContainer::RelationContainer(void *data_FirstDataSet,
                                                    void *data_SecondDataSet)
    : data_FirstDataSet(data_FirstDataSet),
      data_SecondDataSet(data_SecondDataSet) {}

void RelationLayer::OneToOneRelation::addRelation(void *dataFirstDataSet,
                                                  void *dataSecondDataSet) {
  relations.emplace_back(dataFirstDataSet, dataSecondDataSet);
}

void RelationLayer::OneToOneRelation::removeRelation(void *dataFirstDataSet,
                                                     void *dataSecondDataSet) {
  throw NotImplementedException();
}

const std::vector<RelationLayer::RelationContainer> &RelationLayer::OneToOneRelation::findRelations(
    void *pointerToData) {
  throw NotImplementedException();
}

void RelationLayer::OneToNRelation::addRelation(void *dataFirstDataSet,
                                                void *dataSecondDataSet) {
  relations.emplace_back(dataFirstDataSet, dataSecondDataSet);
}

void RelationLayer::OneToNRelation::removeRelation(void *dataFirstDataSet,
                                                   void *dataSecondDataSet) {
  throw NotImplementedException();
}

const std::vector<RelationLayer::RelationContainer> &RelationLayer::OneToNRelation::findRelations(
    void *pointerToData) {
  throw NotImplementedException();
}

void RelationLayer::NToNRelation::addRelation(void *dataFirstDataSet,
                                              void *dataSecondDataSet) {
  relations.emplace_back(dataFirstDataSet, dataSecondDataSet);
}

void RelationLayer::NToNRelation::removeRelation(void *dataFirstDataSet,
                                                 void *dataSecondDataSet) {
  throw NotImplementedException();
}

const std::vector<RelationLayer::RelationContainer> &RelationLayer::NToNRelation::findRelations(
    void *pointerToData) {
  throw NotImplementedException();
}
