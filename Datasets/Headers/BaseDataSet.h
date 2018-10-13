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

namespace DataSets {
class BaseField;
/**
 * Rozhrani pro objekt dodavajici data.
 */
class BaseDataSet {
 protected:
  friend class BaseField;  //< kvuli pristupu k BaseField::setData(...)

  std::vector<BaseField *> fields;  //< Pole umoznujici pristup k datum

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
  virtual void setData(void *data, uint64_t index, ValueType type) = 0;

 public:
  /**
   * Nacteni dat datasetu z IDataProvider
   */
  virtual void open() = 0;

  /**
   * Nastaveni IDataProvider dodavajici data
   * @param provider
   */
  virtual void setDataProvider(DataProviders::BaseDataProvider *provider) = 0;

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
  virtual void next() = 0;

  /**
   * Presun na predchazejici polozku datasetu
   */
  virtual void previous() = 0;

  /**
   * Kontrola pruchodu vsemi zaznamy.
   * @return false pokud neni dostupny zadny zaznam, jinak true
   */
  virtual bool eof() = 0;

  /**
   * Field podle nazvu sloupce
   * @param name Nazev sloupce/Field
   * @return
   */
  virtual BaseField *fieldByName(const std::string &name) = 0;

  /**
   * Field podle jeho indexu v zaznamu
   * @param index
   * @return
   */
  virtual BaseField *fieldByIndex(uint64_t index) = 0;

  /**
   * Nastaveni typu Fields.
   * Vytvori objekty IFields.
   * @param types
   */
  virtual void setFieldTypes(std::vector<ValueType> types) = 0;

  /**
   * Nazvy vsech sloupcu v datasetu
   * @return
   */
  virtual std::vector<std::string> getFieldNames() = 0;

  /**
   * Vytvoreni noveho zaznamu v datasetu
   */
  virtual void append() = 0;

  /**
   * Smazani vsech alokovanych Fields
   */
  virtual ~BaseDataSet();
};
}  // namespace DataSets

#endif //  DATASETS_HEADERS_BASEDATASET_H_
