//
// Created by Petr Flajsingr on 2019-01-29.
//

#ifndef CSV_READER_BASERELATION_H
#define CSV_READER_BASERELATION_H

#include <vector>
#include <Types.h>

namespace RelationLayer {

/**
 * Typy relaci, podobne jako v SQL databazich
 */
enum RelationType {
  OneToOne, OneToN, NTom
};

/**
 * Kontejner obsahujici odkazy na propojena data
 */
struct RelationContainer {
  void *data_FirstDataSet;
  void *data_SecondDataSet;

  RelationContainer(void *data_FirstDataSet, void *data_SecondDataSet);
};

/**
 * Zaobaluje relace mezi tabulkami. Vnitrni struktura uklada dvojice void* pro
 * odkazovani na data - muze byt pouzit libovolny pointer, je nutne nasledne
 * pretypovani.
 */
class Relation {
 private:
  RelationType relationType;

 protected:
  std::vector<RelationContainer> relations;

 public:
  explicit Relation(RelationType relationType);

  virtual ~Relation() = 0;

  /**
   * Pridani relace do seznamu
   * @param dataFirstDataSet odkaz na zaznam prvniho data setu
   * @param dataSecondDataSet odkaz na zaznam druheho data setu
   */
  virtual void addRelation(void *dataFirstDataSet, void *dataSecondDataSet);

  /**
   * Smazani vybraneho zaznamu ze seznamu
   * @param dataFirstDataSet odkaz na zaznam prvniho data setu
   * @param dataSecondDataSet odkaz na zaznam druheho data setu
   */
  virtual void removeRelation(void *dataFirstDataSet,
                              void *dataSecondDataSet);

  /**
   * Nalezeni vsech relaci, ve kterych figuruje poskytnuta hodnota.
   * @param pointerToData vyhledavana hodnota
   * @return vector nalezenych relaci
   */
  virtual const std::vector<RelationContainer> *findRelations(void *pointerToData);

  RelationType getRelationType() const;

  const std::vector<RelationContainer> &getRelations() const;
};

}  // namespace RelationLayer

#endif //CSV_READER_BASERELATION_H
