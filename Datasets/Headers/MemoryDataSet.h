//
// Created by Petr Flajsingr on 25/08/2018.
//

#ifndef DATASETS_HEADERS_MEMORYDATASET_H_
#define DATASETS_HEADERS_MEMORYDATASET_H_

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <thread>

#include <BaseDataSet.h>
#include <MemoryDataWorker.h>
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
/**
 * Struktura pro jeden zaznam v data setu.
 */
struct DataSetRow {
  bool valid;  //< true pokud vyhovuje filtru/neni filtrovat, jinak false
  std::vector<DataContainer> cells;  //< data v pameti
};
/**
 * Dataset ukladajici data primo v operacni pameti.
 *
 *
 * Ukazka pouziti:
    auto dataSet = new DataSets::MemoryDataSet("dataSetName");
    dataSet->setDataProvider(dataProvider);
    dataSet->setFieldTypes({ValueType::IntegerValue, ValueType::CurrencyValue});
    dataSet->open();

    auto fieldID = dynamic_cast<DataSets::IntegerField *>(dataSet->fieldByName("ID"));
    auto fieldWage = dynamic_cast<DataSets::CurrencyField *>(dataSet->fieldByName("wage"));

    DataSets::SortOptions sortOptions;
    sortOptions.addOption(fieldID->getIndex(), SortOrder::Ascending);
    sortOptions.addOption(fieldWage->getIndex(), SortOrder::Descending);
    dataSet->sort(sortOptions);

    DataSets::FilterOptions filterOptions;
    filterOptions.addOption(fieldID->getIndex(),
                            ValueType::IntegerValue,
                            {{._integer=10}},
                            DataSets::FilterOption::EQUALS);
    dataSet->filter(filterOptions);

    while (dataSet->eof()) {
      auto id = fieldID->getAsInteger();
      auto wage = fieldWage->getAsCurrency();

      auto newWage = wage + dec::decimal_cast<2>(10);
      fieldWage->setAsCurrency(newWage);

      dataSet->next();
    }

    dataSet->close();
    delete dataSet;
 */
class MemoryDataSet : public BaseDataSet {
 public:
  explicit MemoryDataSet(std::string_view dataSetName);

  ~MemoryDataSet() override;

  void open(DataProviders::BaseDataProvider &dataProvider,
            const std::vector<ValueType> &fieldTypes) override;

  void openEmpty(const std::vector<std::string> &fieldNames,
                 const std::vector<ValueType> &fieldTypes) override;

  void close() override;

  void first() override;

  void last() override;

  bool next() override;

  bool previous() override;

  void sort(SortOptions &options) override;

  void filter(const FilterOptions &options) override;

  BaseField *fieldByName(std::string_view name) const override;

  BaseField *fieldByIndex(gsl::index index) const override;

  std::vector<BaseField *> getFields() const override;

  bool isFirst() const override;

  bool isLast() const override;

  std::vector<std::string> getFieldNames() const override;

  void append() override;

  void append(DataProviders::BaseDataProvider &dataProvider) override;

  bool findFirst(FilterItem &item) override;

  bool isBegin() const override;

  bool isEnd() const override;

  gsl::index getCurrentRecord() const override;

  void resetBegin() override;
  void resetEnd() override;

 protected:
  void setData(void *data, gsl::index index, ValueType type) override;

 private:
  /**
   * Struktura pro vnitrni reprezentaci dat
   */

  bool isOpen = false;

  const DataSetRow stopItem{true, {}};

  gsl::index currentRecord = 0;  //< Pocitadlo zaznamu

  bool dataValidityChanged = false;
  //< Nastaveno pri zmene dat naprikald pomoci find

  std::vector<DataSetRow> data;

  inline gsl::index getFirst() const;
  inline gsl::index getLast() const;

  /**
   * Nacteni dat do this->data
   */
  void loadData(DataProviders::BaseDataProvider &dataProvider);

  /**
   * Pridani zaznamu do this->data()
   */
  void addRecord(DataProviders::BaseDataProvider &dataProvider);

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
  bool setFieldValues(gsl::index index, bool searchForward);
};
}  // namespace DataSets

#endif //  DATASETS_HEADERS_MEMORYDATASET_H_
