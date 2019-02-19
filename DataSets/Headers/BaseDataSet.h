//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef DATASETS_HEADERS_BASEDATASET_H_
#define DATASETS_HEADERS_BASEDATASET_H_

#include <string>
#include <vector>
#include <cstring>
#include "BaseDataProvider.h"
#include "Types.h"
#include "FilterStructures.h"
#include "SortStructures.h"

namespace DataSets {
class BaseField;
class ViewDataSet;
/**
 * Rozhrani pro objekt dodavajici data.
 */
class BaseDataSet {
 public:
  explicit BaseDataSet(std::string_view dataSetName)
      : dataSetName(dataSetName) {};

  /**
   * Nacteni dat datasetu z IDataProvider
   */
  virtual void open(DataProviders::BaseDataProvider &dataProvider,
                    const std::vector<ValueType> &fieldTypes) = 0;

  /**
   * Otevreni prazdneho datasetu
   */
  virtual void openEmpty(const std::vector<std::string> &fieldNames,
                         const std::vector<ValueType> &fieldTypes) = 0;

  /**
   * Uzavreni datasetu a znepristupneni jeho dat
   */
  virtual void close() = 0;

  /**
   * Presun na prvni polozku datasetu
   */
  virtual void first() = 0;

  /**
   * Presun na posledni polozku datasetu
   */
  virtual void last() = 0;

  /**
   * Presun na nasledujici polozku datasetu
   */
  virtual bool next() = 0;

  /**
   * Presun na predchazejici polozku datasetu
   */
  virtual bool previous() = 0;

  /**
   * Kontrola pruchodu vsemi zaznamy.
   * @return false pokud neni dostupny zadny dalsi zaznam pomoci previous(), jinak true
   */
  virtual bool isFirst() const = 0;

  virtual bool isBegin() const = 0;

  /**
   * Kontrola pruchodu vsemi zaznamy.
   * @return false pokud neni dostupny zadny dalsi zaznam pomoci next(), jinak true
   */
  virtual bool isLast() const = 0;

  virtual bool isEnd() const = 0;

  /**
   * Field podle nazvu sloupce
   * @param name Nazev sloupce/Field
   * @return
   */
  virtual BaseField *fieldByName(std::string_view name) const = 0;

  /**
   * Field podle jeho indexu v zaznamu
   * @param index
   * @return
   */
  virtual BaseField *fieldByIndex(gsl::index index) const = 0;

  /**
   * Vsechny fields
   * @return
   */
  virtual std::vector<BaseField *> getFields() const = 0;

  /**
   * Nazvy vsech sloupcu v datasetu
   * @return
   */
  virtual std::vector<std::string> getFieldNames() const = 0;

  /**
   *
   * @return pocet sloupcu v data setu
   */
  gsl::index getColumnCount() const;

  virtual gsl::index getCurrentRecord() const = 0;

  /**
   * Vytvoreni noveho zaznamu v datasetu
   */
  virtual void append() = 0;

  virtual void append(DataProviders::BaseDataProvider &dataProvider) = 0;

  /**
   * Seradi data set podle dodaneho nastaveni.
   * Umoznuje razeni podle vice klicu - priorita podle poradi v options.
   *
   * @param options
   */
  virtual void sort(SortOptions &options) = 0;

  /**
   * Filtruje data podle dodaneho nastaveni.
   * Lze filtrovat podle vice klicu i podle vice hodnot v ramci jednoho klice.
   *
   * @param options
   */
  virtual std::shared_ptr<ViewDataSet> filter(const FilterOptions &options) = 0;

  /**
   * Najde prvni zaznam odpovidajici zadane polozce.
   * Predpoklada serazeni data setu podle sloupce, ve kterem se vyhledava.
   * Nalezena hodnota je dostupna pres Fields v momentalnim zaznamu.
   *
   * @param item polozka, ktera se ma vyhledam
   * @return true pokud byl zazna nalezen, jinak false
   */
  virtual bool findFirst(FilterItem &item) = 0;

  virtual void resetBegin() = 0;
  virtual void resetEnd() = 0;

  void setName(const std::string &name) {
    dataSetName = name;
  }

  std::string getName() const {
    return dataSetName;
  }

  /**
   * Smazani vsech alokovanych Fields
   */
  virtual ~BaseDataSet() = default;

 protected:
  friend class BaseField;  //< kvuli pristupu k BaseField::setData(...)

  std::vector<std::shared_ptr<BaseField>>
      fields;  //< Pole umoznujici pristup k datum

  std::string dataSetName;

  unsigned long columnCount = 0;

  /**
   * Nastaveni dat pole.
   *
   * Zpristupnuje IField::setData potomkum
   * @param field Field, kteremu budou nastavena data
   * @param data Data pro zapis
   */
  void setFieldData(BaseField *field, void *data);

  /**
   * Nastaveni dat uvnitr datasetu.
   *
   * Volano pri externi zmene dat Field
   * @param data Data k zapisu
   * @param index Index v zaznamu
   * @param type Typ dat
   */
  virtual void setData(void *data, gsl::index index, ValueType type) = 0;
};
}  // namespace DataSets

#endif //  DATASETS_HEADERS_BASEDATASET_H_
