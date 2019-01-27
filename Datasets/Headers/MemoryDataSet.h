//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef DATASETS_HEADERS_MEMORYDATASET_H_
#define DATASETS_HEADERS_MEMORYDATASET_H_

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "BaseDataSet.h"
#include "Types.h"
#include "FilterStructures.h"
#include "Exceptions.h"
#include "IntegerField.h"
#include "StringField.h"
#include "DoubleField.h"
#include "Utilities.h"
#include "CurrencyField.h"
#include <BaseDataProvider.h>
#include <DateTimeField.h>

namespace DataSets {

typedef std::vector<DataContainer *> DataSetRowCells;

/**
 * Struktura pro jeden zaznam v data setu.
 */
struct DataSetRow {
  bool valid;
  DataSetRowCells *cells;
};

typedef std::vector<DataSetRow *> DataSetData;
/**
 * Dataset ukladajici data primo v operacni pameti.
 */
class MemoryDataSet : public BaseDataSet {
 private:
  /**
   * Struktura pro vnitrni reprezentaci dat
   */

  //\

  DataProviders::BaseDataProvider *dataProvider = nullptr;  //< dodavatel dat

  bool isOpen = false;

  uint64_t currentRecord = 0;  //< Pocitadlo zaznamu

  bool dataValidityChanged = false;
  //< Nastaveno pri zmene dat naprikald pomoci find

  DataSetData data;

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
  MemoryDataSet(const std::string &dataSetName);

  ~MemoryDataSet() override;

  void open() override;

  void openEmpty() override;

  void setDataProvider(DataProviders::BaseDataProvider *provider) override;

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
  void sort(SortOptions &options) override;

  /**
   * Vyhledani zaznamu podle zadanych klicu
   * @param options
   */
  void filter(const FilterOptions &options) override;

  BaseField *fieldByName(const std::string &name) override;

  BaseField *fieldByIndex(uint64_t index) override;

  std::vector<BaseField *> getFields() override;

  bool eof() override;

  std::vector<std::string> getFieldNames() override {
    std::vector<std::string> result;

    for (const auto &field : fields) {
      result.push_back(field->getFieldName());
    }

    return result;
  }

  void setFieldTypes(std::vector<ValueType> types) override;

  void setFieldTypes(std::vector<std::string> fieldNames, std::vector<ValueType> types) override;

  /**
   * Pridani prazdneho zaznamu na konec data setu.
   */
  void append() override;

  virtual void appendDataProvider(DataProviders::BaseDataProvider *provider);

  bool findFirst(FilterItem &item) override;
};
}  // namespace DataSets

#endif //  DATASETS_HEADERS_MEMORYDATASET_H_
