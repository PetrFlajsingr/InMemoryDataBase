//
// Created by Petr Flajsingr on 2019-01-29.
//

#ifndef CSV_READER_BASERELATION_H
#define CSV_READER_BASERELATION_H

#include <iterator>
#include <vector>
#include <Types.h>
#include <mach/machine.h>
#include <MemoryDataSet.h>

namespace DataBase {

/**
 * Typy relaci, podobne jako v SQL databazich
 */
enum class RelationType {
  OneToOne, OneToN, NToM
};

/**
 * Kontejner obsahujici odkazy na propojena data
 */
struct RelationContainer {
  DataSetRow *data_FirstDataSet;
  DataSetRow *data_SecondDataSet;
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
  /**
   * Random access iterator.
   */
  class iterator : std::iterator<std::random_access_iterator_tag,
                                 RelationContainer> {
   private:
    Relation *relation;
    uint64_t position;  //< pozice v poli

   public:
    iterator() = default;

    explicit iterator(Relation *relation, uint64_t position)
        : relation(relation), position(position) {}

    iterator(const iterator &other) {
      relation = other.relation;
      position = other.position;
    }

    /**
     * Prepis hodnoty
     * @param rhs
     * @return
     */
    iterator &operator=(const RelationContainer &rhs) {
      relation->relations[position] = rhs;
      return *this;
    }

    /**
     * Posun vpred
     * @return
     */
    iterator &operator++() {
      if (position < relation->relations.size()) {
        position++;
      }
      return *this;
    }

    /**
     * Posun vpred
     * @return
     */
    const iterator operator++(int) {
      iterator result = *this;
      ++(*this);
      return result;
    }

    /**
     * Srovnani pozice
     * @param rhs
     * @return
     */
    bool operator==(const iterator &rhs) const {
      return position == rhs.position;
    }

    /**
     * Srovnani pozice
     * @param rhs
     * @return
     */
    bool operator!=(const iterator &rhs) const {
      return !(rhs == *this);
    }

    /**
     * Dereference na ulozenou hodnotu
     * @return
     */
    RelationContainer &operator*(int) {
      return relation->relations[position];
    }

    iterator *operator->() {
      return this;
    }

    /**
     * Posun vzad
     * @return
     */
    iterator &operator--() {
      if (position > 0) {
        position--;
      }
      return *this;
    }

    /**
     * Posun vzad
     * @return
     */
    const iterator operator--(int) {
      iterator result = *this;
      --(*this);
      return result;
    }

    /**
     * Posun o libovolny krok vpred
     * @param rhs
     * @return
     */
    iterator operator+(const uint64_t rhs) {
      iterator result = *this;
      result.position += rhs;

      if (result.position > relation->relations.size()) {
        result.position = relation->relations.size();
      }

      return result;
    }

    /**
     * Posun o libovolný krok vzad
     * @param rhs
     * @return
     */
    iterator operator-(const uint64_t rhs) {
      iterator result = *this;
      if (rhs > result.position) {
        result.position = 0;
      } else {
        result.position -= rhs;
      }
      return result;
    }

    /**
     * Vzdalenost dvou iteratoru
     * @param rhs
     * @return
     */
    iterator operator-(const iterator &rhs) {
      auto result = *this;
      result.position -= rhs.position;
      return result;
    }

    /**
     * Srovnani pozice
     * @param rhs
     * @return
     */
    bool operator<(const iterator &rhs) {
      return position < rhs.position;
    }

    /**
     * Srovnani pozice
     * @param rhs
     * @return
     */
    bool operator>(const iterator &rhs) {
      return position > rhs.position;
    }

    /**
     * Srovnani pozice
     * @param rhs
     * @return
     */
    bool operator<=(const iterator &rhs) {
      return position <= rhs.position;
    }

    /**
     * Srovnani pozice
     * @param rhs
     * @return
     */
    bool operator>=(const iterator &rhs) {
      return position >= rhs.position;
    }

    /**
     * Posun vpred o libovolnou hodnotu
     * @param rhs
     * @return
     */
    iterator &operator+=(const uint64_t rhs) {
      position += rhs;
      return *this;
    }

    /**
     * Posun vzad o libovolnou hodnotu
     * @param rhs
     * @return
     */
    iterator &operator-=(const uint64_t rhs) {
      if (rhs > position) {
        position = 0;
      } else {
        position -= rhs;
      }
      return *this;
    }

    /**
     * Primy pristup k datum
     * @param index
     * @return
     */
    RelationContainer &operator[](const uint64_t index) {
      return relation->relations[index];
    }
  };

  explicit Relation(RelationType relationType);

  virtual ~Relation() = 0;

  /**
   * Pridani relace do seznamu
   * @param dataFirstDataSet odkaz na zaznam prvniho data setu
   * @param dataSecondDataSet odkaz na zaznam druheho data setu
   */
  virtual void addRelation(DataSetRow *dataFirstDataSet,
                           DataSetRow *dataSecondDataSet);

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
  virtual const std::vector<RelationContainer> *findRelations(void *pointerToData) const;

  RelationType getRelationType() const;

  const std::vector<RelationContainer> &getRelations() const;
};

}  // namespace DataBase

#endif //CSV_READER_BASERELATION_H
