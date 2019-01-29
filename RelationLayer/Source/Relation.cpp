//
// Created by Petr Flajsingr on 2019-01-29.
//

#include <Relation.h>

RelationLayer::RelationContainer::RelationContainer(void *data_FirstDataSet,
                                                    void *data_SecondDataSet)
    : data_FirstDataSet(data_FirstDataSet),
      data_SecondDataSet(data_SecondDataSet) {}

RelationLayer::Relation::Relation(RelationLayer::RelationType relationType)
    : relationType(relationType) {}

void RelationLayer::Relation::addRelation(void *dataFirstDataSet,
                                          void *dataSecondDataSet) {
  relations.emplace_back(dataFirstDataSet, dataSecondDataSet);
}

void RelationLayer::Relation::removeRelation(void *dataFirstDataSet,
                                             void *dataSecondDataSet) {
  auto result = std::find_if(relations.begin(),
                             relations.end(),
                             [dataFirstDataSet, dataSecondDataSet](
                                 RelationContainer &container) {
                               return container.data_FirstDataSet
                                   == dataFirstDataSet &&
                                   container.data_SecondDataSet
                                       == dataSecondDataSet;
                             });

  if (result != relations.end()) {
    relations.erase(result);
  }
}

const std::vector<RelationLayer::RelationContainer> *RelationLayer::Relation::findRelations(
    void *pointerToData) {
  auto result = new std::vector<RelationContainer>();
  std::copy_if(relations.begin(),
               relations.end(),
               std::back_inserter(*result),
               [pointerToData](RelationContainer &container) {
                 return container.data_SecondDataSet == pointerToData
                     || container.data_FirstDataSet == pointerToData;
               });

  return result;
}

RelationLayer::RelationType RelationLayer::Relation::getRelationType() const {
  return relationType;
}

const std::vector<RelationLayer::RelationContainer> &RelationLayer::Relation::getRelations() const {
  return relations;
}
