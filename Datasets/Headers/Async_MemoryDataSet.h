//
// Created by Petr Flajsingr on 02/09/2018.
//

#ifndef DATASETS_HEADERS_ASYNC_MEMORYDATASET_H_
#define DATASETS_HEADERS_ASYNC_MEMORYDATASET_H_

#include "MemoryDataSet.h"
#include <vector>
#include <algorithm>

// TODO: implementovat dalsi udalosti, vytvořit zamky, frontu pozadavku?
namespace DataSets {
class IAsyncMemoryDataSetObserver;

/**
 * Modifikace data setu v pameti, pracuje asynchronne.
 * Vyvolava udalosti pred/po dokonceni nejake operace.
 *
 * Udalosti jsou vyvolany ve stejnem vlakne, ve kterem pracuje tento data set.
 *
 * Pouziti obdobne jako MemoryDataSet, krom vyvolanych udalosti
 */
class Async_MemoryDataSet : public MemoryDataSet {
 public:
  explicit Async_MemoryDataSet(std::string_view dataSetName);

  void open() override;

  void filter(const FilterOptions &options) override;

  void sort(SortOptions &options) override;

  void appendDataProvider(DataProviders::BaseDataProvider *provider) override;

  void addObserver(IAsyncMemoryDataSetObserver *observer);

  void removeObserver(IAsyncMemoryDataSetObserver *observer);

 protected:
  std::vector<IAsyncMemoryDataSetObserver *> observers;

  enum class EventType {
    BeforeOpen,
    AfterOpen,
    BeforeSort,
    AfterSort,
    BeforeFilter,
    AfterFilter,
    BeforeAppend,
    AfterAppend
  };

  /**
   * Pro otevreni data setu v dalsim vlakne (std::async)
   */
  void innerOpen();

  /**
   * Pro filtrovani dat v dalsim vlakne (std::async)
   * @param options
   */
  void innerFilter(const FilterOptions &options);

  /**
   * Pro razeni dat v dalsim vlakne (std::async)
   * @param options
   */
  void innerSort(SortOptions &options);

  /**
   * Pro pridani dat z dalsiho provideru ve vlakne (std::async)
   * @param provider
   */
  void innerAppendDataProvider(DataProviders::BaseDataProvider *provider);

  /**
   * Notifikuj posluchace o udalosti daneho typu.
   * TODO: pridat parametr pro preadni posluchacum (uspesne otevreni atp...)
   * @param type
   */
  void notify(EventType type);
};

class IAsyncMemoryDataSetObserver {
 public:
  /**
   * Pred otevrenim data setu.
   * @param sender
   */
  virtual void onBeforeOpen(Async_MemoryDataSet *sender) = 0;

  /**
   * Po otevreni data setu.
   * @param sender
   */
  virtual void onAfterOpen(Async_MemoryDataSet *sender) = 0;

  /**
   * Pred razenim data setu.
   * @param sender
   */
  virtual void onBeforeSort(Async_MemoryDataSet *sender) = 0;

  /**
   * Po dokonceni razeni data setu.
   * @param sender
   */
  virtual void onAfterSort(Async_MemoryDataSet *sender) = 0;

  /**
   * Pred filtrovani data setu.
   * @param sender
   */
  virtual void onBeforeFilter(Async_MemoryDataSet *sender) = 0;

  /**
   * Po dokonceni filtrovani data setu.
   * @param sender
   */
  virtual void onAfterFilter(Async_MemoryDataSet *sender) = 0;

  /**
   * Pred nacteni data provider.
   * @param sender
   */
  virtual void onBeforeAppendDataProvider(Async_MemoryDataSet *sender) = 0;

  /**
   * Po nacteni data provider.
   * @param sender
   */
  virtual void onAfterAppendDataProvider(Async_MemoryDataSet *sender) = 0;

  virtual ~IAsyncMemoryDataSetObserver() = default;
};
}  // namespace DataSets

#endif  // DATASETS_HEADERS_ASYNC_MEMORYDATASET_H_
