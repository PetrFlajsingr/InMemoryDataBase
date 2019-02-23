//
// Created by Petr Flajsingr on 2019-02-08.
//

#ifndef PROJECT_JOINMAKER_H
#define PROJECT_JOINMAKER_H

#include <MemoryViewDataSet.h>
#include <MemoryDataBase.h>
#include <QueryCommon.h>

namespace DataBase {
template<typename T1, typename T2>
class JoinMaker {
  using IteratorType1 = typename std::remove_reference<decltype(*T1::dataSet)>::type::iterator;
  using IteratorType2 = typename std::remove_reference<decltype(*T2::dataSet)>::type::iterator;
  using DataSetType1 = decltype(T1::dataSet);
  using DataSetType2 = decltype(T2::dataSet);

 public:
  JoinMaker(const std::shared_ptr<T1> &t1,
            const std::string &field1,
            const std::shared_ptr<T2> &t2,
            const std::string &field2)
      : t1(t1), t2(t2), col1(field1), col2(field2) {
    static_assert(std::is_same<T1, Table>{} || std::is_same<T1, View>{});
    static_assert(std::is_same<T2, Table>{} || std::is_same<T2, View>{});
  }

  /**
   * Apply desired join operation on provided data
   * @param joinType type of join
   * @return view to joined data
   */
  std::shared_ptr<View> join(JoinType joinType) {
    auto result = prepareResultView();
    iterAndCompare(t1->dataSet, t2->dataSet,
                   getFirstIndices(),
                   getSecondIndices(),
                   getJoinFunction(result, joinType),
                   getValueType());

    result->rawData()->emplace_back();
    return std::make_shared<View>(result);
  }

 private:
  std::shared_ptr<T1> t1;
  std::shared_ptr<T2> t2;
  std::string col1;
  std::string col2;

  /**
   * Iterate through both datasets and compare its values.
   * @tparam CompFnc function to call on compare
   * @param first first data set
   * @param second second data set
   * @param firstIndex index of field in first data set
   * @param secondIndex index of field in second data set
   * @param cmpFunc compare function
   * @param valueType type of value to compare
   */
  template<typename CompFnc>
  void iterAndCompare(DataSetType1 first,
                      DataSetType2 second,
                      std::pair<gsl::index,
                                gsl::index> firstIndex,
                      std::pair<gsl::index,
                                gsl::index> secondIndex,
                      CompFnc cmpFunc,
                      ValueType valueType) {
    auto begin1 = first->begin();
    auto end1 = first->end();
    auto begin2 = second->begin();
    auto end2 = second->end();

    DataContainer container1, container2;
    bool found = false;
    int diff = 0;
    while (begin1 != end1) {
      while (begin2 != end2) {
        if constexpr(std::is_same<DataSetType1,
                                  std::shared_ptr<DataSets::MemoryViewDataSet>>{}) {
          container1 = (*(*begin1)[firstIndex.first])[firstIndex.second];
        }
        if constexpr(std::is_same<DataSetType1,
                                  std::shared_ptr<DataSets::MemoryDataSet>>{}) {
          container1 = (*(*begin1))[firstIndex.first];
        }

        if constexpr(std::is_same<DataSetType2,
                                  std::shared_ptr<DataSets::MemoryViewDataSet>>{}) {
          container2 = (*(*begin2)[secondIndex.first])[secondIndex.second];
        }
        if constexpr(std::is_same<DataSetType2,
                                  std::shared_ptr<DataSets::MemoryDataSet>>{}) {
          container2 = (*(*begin2))[secondIndex.first];
        }

        auto cmpResult =
            compareDataContainers(container1, container2, valueType);

        cmpFunc(cmpResult, found, begin1, begin2);
        if (cmpResult == 0) {
          found = true;
          ++diff;
        } else if (cmpResult == -1) {
          goto outer_loop;
        }

        ++begin2;
      }
      outer_loop:
      ++begin1;
      if (found) {
        found = false;
        begin2 -= diff;
        diff = 0;
      }
    }
  }

  /**
   * Creates fields for result of join. Sorts data sets before joining them.
   * @return empty MemoryViewDataSet
   */
  std::shared_ptr<DataSets::MemoryViewDataSet> prepareResultView() {
    DataSets::BaseDataSet *dataSet1 = t1->dataSet.get();
    DataSets::BaseDataSet *dataSet2 = t2->dataSet.get();

    const auto field1 = dataSet1->fieldByName(col1);
    DataSets::SortOptions options1;
    options1.addOption(field1, SortOrder::Ascending);
    dataSet1->sort(options1);

    const auto field2 = dataSet2->fieldByName(col2);
    DataSets::SortOptions options2;
    options2.addOption(field2, SortOrder::Ascending);
    dataSet2->sort(options2);

    std::vector<std::string> fieldNames;
    std::vector<ValueType> fieldTypes;
    std::vector<std::pair<int, int>> fieldIndices;
    auto fields1 = dataSet1->getFields();
    gsl::index offset = 1;
    if (auto view = dynamic_cast<DataSets::MemoryViewDataSet *>(dataSet1); view
        != nullptr) {
      offset = (*view->rawData())[1].size();
    }
    std::for_each(fields1.begin(),
                  fields1.end(),
                  [&fieldNames, &fieldTypes, &fieldIndices](const DataSets::BaseField *field) {
                    fieldNames.emplace_back(field->getName());
                    fieldTypes.emplace_back(field->getFieldType());
                    fieldIndices.emplace_back(0, field->getIndex());
                  });
    auto fields2 = dataSet2->getFields();
    std::for_each(fields2.begin(),
                  fields2.end(),
                  [&offset, &fieldNames, &fieldTypes, &fieldIndices](const DataSets::BaseField *field) {
                    fieldNames.emplace_back(field->getName());
                    fieldTypes.emplace_back(field->getFieldType());
                    fieldIndices.emplace_back(offset, field->getIndex());
                  });

    auto result = std::make_shared<DataSets::MemoryViewDataSet>(
        "join_" + dataSet1->getName() + "_" + dataSet2->getName(),
        fieldNames,
        fieldTypes,
        fieldIndices);
    result->rawData()->emplace_back();
    return result;
  }

  /**
   * Get converted indices of first field <tableIndex, columnIndex> or <columnIndex, 0>
   * @return indices of first field
   */
  std::pair<gsl::index, gsl::index> getFirstIndices() {
    if constexpr (std::is_same<DataSetType1,
                               std::shared_ptr<DataSets::MemoryDataSet>>{}) {
      return std::make_pair(t1->dataSet->fieldByName(col1)->getIndex(), 0);
    }

    if constexpr (std::is_same<DataSetType1,
                               std::shared_ptr<DataSets::MemoryViewDataSet>>{}) {
      return DataSets::BaseField::convertIndex(*t1->dataSet->fieldByName(col1));
    }
  }

  /**
   * Get converted indices of second field <tableIndex, columnIndex> or <columnIndex, 0>
   * @return indices of second field
   */
  std::pair<gsl::index, gsl::index> getSecondIndices() {
    if constexpr (std::is_same<DataSetType2,
                               std::shared_ptr<DataSets::MemoryDataSet>>{}) {
      return std::make_pair(t2->dataSet->fieldByName(col2)->getIndex(), 0);
    }

    if constexpr (std::is_same<DataSetType2,
                               std::shared_ptr<DataSets::MemoryViewDataSet>>{}) {
      return DataSets::BaseField::convertIndex(*t2->dataSet->fieldByName(col2));
    }
  }

  ValueType getValueType() {
    return t1->dataSet->fieldByName(col1)->getFieldType();
  }

  /**
   * Get join function based on template type and join type
   * @param result result view to capture in lambda
   * @param joinType type of join operation
   * @return
   */
  std::function<void(int8_t, bool,
                     IteratorType1 &,
                     IteratorType2 &)>
  getJoinFunction(std::shared_ptr<DataSets::MemoryViewDataSet> result,
                  JoinType joinType) {
    if constexpr (std::is_same<T1, Table>{}) {
      if constexpr (std::is_same<T2, Table>{}) {
        switch (joinType) {
          case JoinType::innerJoin:
            return [&result]
                (int8_t cmpResult,
                 bool,
                 IteratorType1 &iter1,
                 IteratorType2 &iter2) {
              if (cmpResult == 0) {
                result->rawData()->emplace_back(std::vector<DataSetRow *>{
                    *iter1, *iter2});
              }
            };
          case JoinType::leftJoin:
            return [&result]
                (int8_t cmpResult,
                 bool found,
                 IteratorType1 &iter1,
                 IteratorType2 &iter2) {
              if (cmpResult == 0) {
                result->rawData()->emplace_back(std::vector<DataSetRow *>{
                    *iter1, *iter2});
              } else if (cmpResult == -1 && !found) {
                result->rawData()->emplace_back(std::vector<DataSetRow *>{
                    *iter1, result->getNullRow(result->getTableCount() - 1)});
              }
            };
          case JoinType::rightJoin:break;
          case JoinType::outerJoin:break;
        }
      }
    }
    if constexpr (std::is_same<T1, View>{}) {
      if constexpr (std::is_same<T2, Table>{}) {
        switch (joinType) {
          case JoinType::innerJoin:
            return [&result]
                (int8_t cmpResult,
                 bool,
                 IteratorType1 &iter1,
                 IteratorType2 &iter2) {
              if (cmpResult == 0) {
                std::vector<DataSetRow *> newRecord;
                std::copy((*iter1).begin(),
                          (*iter1).end(),
                          std::back_inserter(newRecord));
                newRecord.emplace_back(*iter2);
                result->rawData()->emplace_back(newRecord);
              }
            };
          case JoinType::leftJoin:
            return [&result]
                (int8_t cmpResult,
                 bool found,
                 IteratorType1 &iter1,
                 IteratorType2 &iter2) {
              if (cmpResult == 0) {
                std::vector<DataSetRow *> newRecord;
                std::copy((*iter1).begin(),
                          (*iter1).end(),
                          std::back_inserter(newRecord));
                newRecord.emplace_back(*iter2);
                result->rawData()->emplace_back(newRecord);
              } else if (cmpResult == -1 && !found) {
                std::vector<DataSetRow *> newRecord;
                std::copy((*iter1).begin(),
                          (*iter1).end(),
                          std::back_inserter(newRecord));
                newRecord.emplace_back(result->getNullRow(
                    result->getTableCount() - 1));
                result->rawData()->emplace_back(newRecord);
              }
            };
          case JoinType::rightJoin:break;
          case JoinType::outerJoin:break;
        }
      }
      if constexpr (std::is_same<T2, View>{}) {
        switch (joinType) {
          case JoinType::innerJoin:
            return [&result]
                (int8_t cmpResult,
                 bool,
                 IteratorType1 &iter1,
                 IteratorType2 &iter2) {
              if (cmpResult == 0) {
                std::vector<DataSetRow *> newRecord;
                std::copy((*iter1).begin(),
                          (*iter1).end(),
                          std::back_inserter(newRecord));
                std::copy((*iter2).begin(),
                          (*iter2).end(),
                          std::back_inserter(newRecord));
                result->rawData()->emplace_back(newRecord);
              }
            };
          case JoinType::leftJoin:
            return [&result]
                (int8_t cmpResult,
                 bool found,
                 IteratorType1 &iter1,
                 IteratorType2 &iter2) {
              if (cmpResult == 0) {
                std::vector<DataSetRow *> newRecord;
                std::copy((*iter1).begin(),
                          (*iter1).end(),
                          std::back_inserter(newRecord));
                std::copy((*iter2).begin(),
                          (*iter2).end(),
                          std::back_inserter(newRecord));
                result->rawData()->emplace_back(newRecord);
              } else if (cmpResult == -1 && !found) {
                std::vector<DataSetRow *> newRecord;
                std::copy((*iter1).begin(),
                          (*iter1).end(),
                          std::back_inserter(newRecord));
                newRecord.emplace_back(result->getNullRow(
                    result->getTableCount() - 1));
                result->rawData()->emplace_back(newRecord);
              }
            };
          case JoinType::rightJoin:break;
          case JoinType::outerJoin:break;
        }
      }
    }
  }
};
}

#endif //PROJECT_JOINMAKER_H
