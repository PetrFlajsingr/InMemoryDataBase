//
// Created by Petr Flajsingr on 27/08/2018.
//

#ifndef DATASETS_FIELDS_HEADERS_BASEFIELD_H_
#define DATASETS_FIELDS_HEADERS_BASEFIELD_H_

#include <string>
#include <utility>
#include <functional>
#include "Types.h"
#include "Exceptions.h"

namespace DataSets {
class BaseDataSet;
struct DataSetRow;

/**
 * Interface pro fields datasetu.
 */
class BaseField {
 public:
  /**
   * Nastaveni datasetu, nazvu a indexu field
   * @param fieldName Nazev field
   * @param dataset Rodicovsky dataset
   * @param index Index pole v zaznamu
   */
  explicit BaseField(std::string_view fieldName,
                     gsl::not_null<BaseDataSet *> dataset,
                     uint64_t index) : fieldName(fieldName),
                                       index(index),
                                       dataSet(dataset) {}

  virtual ~BaseField() = default;

  /**
   * Typ dat ulozenych ve Field
   * @return
   */
  virtual ValueType getFieldType() const = 0;

  /**
   * Nastaveni hodnoty pole pomoci string
   * @param value
   */
  virtual void setAsString(std::string_view value) = 0;

  /**
   * Navrat hodnoty v poli jako string
   * @return
   */
  virtual std::string getAsString() const = 0;

  /**
   *
   * @return Index Field v DataSet
   */
  uint64_t getIndex() const {
    return index;
  }

  /**
   *
   * @return Nazev pole
   */
  std::string_view getFieldName() const {
    return fieldName;
  }

  /**
   * Funkce pro razeni polozek datasetu podle jejich datoveho typu
   * @param order poradi (ascending nebo descending)
   * @return porovnavaci funkce, ktera vraci:
   *    0 pokud se prvky rovnaji
   *    1 pokud je prvni vetsi
   *    -1 pokud je prvni mensi
   */
  virtual std::function<int8_t(DataSetRow *,
                               DataSetRow *)> getCompareFunction() = 0;

 protected:
  std::string fieldName;  //< Nazev reprezentovaneho sloupce

  uint64_t index;  //< Index sloupce

  friend class BaseDataSet;  //< Pro pristup k primemu nastaveni dat

  BaseDataSet *dataSet;  //< Vlastnik

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
  void setData(void *data, ValueType type);
};
}  // namespace DataSets

#endif  // DATASETS_FIELDS_HEADERS_BASEFIELD_H_
