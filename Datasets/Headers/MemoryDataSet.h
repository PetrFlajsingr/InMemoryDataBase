//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef DATASETS_HEADERS_MEMORYDATASET_H_
#define DATASETS_HEADERS_MEMORYDATASET_H_

#include "IDataSet.h"
#include "Types.h"
#include "IField.h"
#include "FilterStructures.h"
#include "Exceptions.h"
#include "IntegerField.h"
#include "StringField.h"
#include "DoubleField.h"
#include "Utilities.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

namespace DataSets {
/**
 * Dataset ukladajici data primo v operacni pameti.
 */
class MemoryDataSet : public IDataSet {
 private:
  /**
   * Struktura pro vnitrni reprezentaci dat
   */
  union InnerData {
    char *_string = nullptr;
    int _integer;
    double _double;
  };

  struct DataContainer {
    InnerData data;
  };
  //\

  IDataProvider *dataProvider = nullptr;  //< dodavatel dat

  bool isOpen = false;

  uint64_t currentRecord = 0;  //< Pocitadlo zaznamu

  bool dataValidityChanged = false;
  //< Nastaveno pri zmene dat naprikald pomoci find

  std::vector<std::vector<DataContainer *> *> data;  //< Data uvnitr datasetu
  std::vector<bool> dataValidity;  //< Vyrazeni dat nevyhovujici pozadavkum

  /**
   * Nacteni dat do this->data
   */
  void loadData();

  /**
   * Pridani zaznamu do this->data()
   */
  void addRecord();

  /**
   * Vytvoreni Fields se jmeny podle nazvu sloupcu.
   * @param columns nazvy sloupcu
   */
  void createFields(std::vector<std::string> columns,
                    std::vector<ValueType> types);

  /**
   * Nastaveni hodnot this->fields.
   *
   * Vynechava zaznamy s dataValidity == false
   * @param index Index Field
   * @param searchForward Smer vyhledavani validniho zaznamu
   * @return
   */
  bool setFieldValues(uint64_t index, bool searchForward);

 protected:
  void setData(void *data, uint64_t index, ValueType type) override;

 public:
  ~MemoryDataSet() override;

  void open() override;

  void setDataProvider(IDataProvider *provider) override;

  void close() override;

  void first() override;

  void last() override;

  void next() override;

  void previous() override;

  /**
   * Serazeni hodnot datasetu podle zadanych klicu.
   * Poradi klicu urcuje jejich prioritu.
   * @param options
   */
  virtual void sort(uint64_t fieldIndex, SortOrder sortOrder);

  /**
   * Vyhledani zaznamu podle zadanych klicu
   * @param options
   */
  virtual void filter(const FilterOptions &options);

  IField *fieldByName(const std::string &name) override;

  IField *fieldByIndex(uint64_t index) override;

  bool eof() override;

  std::vector<std::string> getFieldNames() override {
    std::vector<std::string> result;
    for (const auto &field : fields) {
      result.push_back(field->getFieldName());
    }

    return result;
  }

  void setFieldTypes(std::vector<ValueType> types) override;

  void append() override;

  virtual void appendDataProvider(IDataProvider *provider);
};
}
#endif //  DATASETS_HEADERS_MEMORYDATASET_H_
