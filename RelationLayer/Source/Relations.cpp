//
// Created by Petr Flajsingr on 2019-01-29.
//

#include <Relations.h>
#include <Exceptions.h>

void RelationLayer::OneToOneRelation::addRelation(void *dataFirstDataSet, void *dataSecondDataSet) {
  throw NotImplementedException();
}

void RelationLayer::OneToOneRelation::removeRelation(void *dataFirstDataSet, void *dataSecondDataSet) {
  throw NotImplementedException();
}

const std::vector<RelationLayer::RelationContainer> &RelationLayer::OneToOneRelation::findRelations(void *pointerToData) {
  throw NotImplementedException();
}

void RelationLayer::OneToNRelation::addRelation(void *dataFirstDataSet, void *dataSecondDataSet) {
  throw NotImplementedException();
}

void RelationLayer::OneToNRelation::removeRelation(void *dataFirstDataSet, void *dataSecondDataSet) {
  throw NotImplementedException();
}

const std::vector<RelationLayer::RelationContainer> &RelationLayer::OneToNRelation::findRelations(void *pointerToData) {
  throw NotImplementedException();
}

void RelationLayer::NToNRelation::addRelation(void *dataFirstDataSet, void *dataSecondDataSet) {
  throw NotImplementedException();
}

void RelationLayer::NToNRelation::removeRelation(void *dataFirstDataSet, void *dataSecondDataSet) {
  throw NotImplementedException();
}

const std::vector<RelationLayer::RelationContainer> &RelationLayer::NToNRelation::findRelations(void *pointerToData) {
  throw NotImplementedException();
}
