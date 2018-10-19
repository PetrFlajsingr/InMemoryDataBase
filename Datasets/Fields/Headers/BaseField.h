//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_FIELDS_HEADERS_BASEFIELD_H_
#define DATASETS_FIELDS_HEADERS_BASEFIELD_H_

#include <string>
#include <utility>
#include "Types.h"
#include "BaseDataSet.h"

namespace DataSets {
class BaseDataSet;
/**
 * Interface pro fields datasetu.
 */
class BaseField {
 protected:
  std::string fieldName;  //< Nazev reprezentovaneho sloupce

  uint64_t index;  //< Index sloupce

 public:
  const std::string &getFieldName() const {
    return fieldName;
  }

 protected:
  friend class BaseDataSet;

  BaseDataSet *dataSet;  //< Rodicovsky dataset

  /**
   * Nastaveni hodnoty field.
   * @param data pointer na data
   */
  virtual void setValue(void *data) = 0;

  /**
   * Nastaveni dat v datasetu.
   *
   * Tato funkce zpristupnuje setValue potomkum.
   * @param data data pro ulozeni
   * @param type typ dat
   */
  void setDataSetData(void *data, ValueType type);

 public:
  /**
   * Nastaveni datasetu, nazvu a indexu field
   * @param fieldName Nazev field
   * @param dataset Rodicovsky dataset
   * @param index Index pole v zaznamu
   */
  explicit BaseField(const std::string& fieldName,
                  BaseDataSet *dataset,
                  uint64_t index) : fieldName(fieldName),
                                    dataSet(dataset),
                                    index(index) {}

  virtual ~BaseField() = default;

  /**
   * Typ dat ulozenych ve Field
   * @return
   */
  virtual ValueType getFieldType() = 0;

  /**
   * Nastaveni hodnoty pole pomoci string
   * @param value
   */
  virtual void setAsString(const std::string &value) = 0;

  /**
   * Navrat hodnoty v poli jako string
   * @return
   */
  virtual std::string getAsString() = 0;

  uint64_t getIndex() {
      return index;
  }
};
}  // namespace DataSets

#endif  // DATASETS_FIELDS_HEADERS_BASEFIELD_H_
