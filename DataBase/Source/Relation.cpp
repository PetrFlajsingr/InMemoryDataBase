//
// Created by Petr Flajsingr on 2019-01-29.
//

#include <Relation.h>

DataBase::Relation::Relation(DataBase::RelationType relationType)
    : relationType(relationType) {}

void DataBase::Relation::addRelation(DataSets::DataSetRow *dataFirstDataSet,
                                     DataSets::DataSetRow *dataSecondDataSet) {
  relations.emplace_back(RelationContainer{dataFirstDataSet,
                                           dataSecondDataSet});
}

void DataBase::Relation::removeRelation(void *dataFirstDataSet,
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

const std::vector<DataBase::RelationContainer> *DataBase::Relation::findRelations(
    void *pointerToData) const {
  auto result = new std::vector<RelationContainer>();
  std::copy_if(relations.begin(),
               relations.end(),
               std::back_inserter(*result),
               [pointerToData](const RelationContainer &container) {
                 return container.data_SecondDataSet == pointerToData
                     || container.data_FirstDataSet == pointerToData;
               });

  return result;
}

DataBase::RelationType DataBase::Relation::getRelationType() const {
  return relationType;
}

const std::vector<DataBase::RelationContainer> &DataBase::Relation::getRelations() const {
  return relations;
}
