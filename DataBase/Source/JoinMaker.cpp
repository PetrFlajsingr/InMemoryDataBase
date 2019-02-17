//
// Created by Petr Flajsingr on 2019-02-08.
//

#include <JoinMaker.h>

DataBase::JoinMaker::JoinMaker(const std::shared_ptr<DataBase::Table> &t1,
                               const std::string &field1,
                               const std::shared_ptr<DataBase::Table> &t2,
                               const std::string &field2)
    : t1(t1),
      col1(field1),
      t2(t2),
      col2(field2),
      mode(JoinMode::DataSetDataSet) {}

DataBase::JoinMaker::JoinMaker(const std::shared_ptr<DataBase::View> &view1,
                               const std::string &field1,
                               const std::shared_ptr<DataBase::Table> &t2,
                               const std::string &field2)
    : view1(view1),
      col1(field1),
      t2(t2),
      col2(field2),
      mode(JoinMode::ViewDataSet) {}

DataBase::JoinMaker::JoinMaker(const std::shared_ptr<DataBase::View> &view1,
                               const std::string &field1,
                               const std::shared_ptr<DataBase::View> &view2,
                               const std::string &field2)
    : view1(view1),
      col1(field1),
      view2(view2),
      col2(field2),
      mode(JoinMode::ViewView) {}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::prepareResultView() {
  DataSets::BaseDataSet *dataSet1;
  DataSets::BaseDataSet *dataSet2;
  switch (mode) {
    case JoinMode::DataSetDataSet:dataSet1 = t1->dataSet.get();
      dataSet2 = t2->dataSet.get();
      break;
    case JoinMode::ViewDataSet:dataSet1 = view1->dataSet.get();
      dataSet2 = t2->dataSet.get();
      break;
    case JoinMode::ViewView:dataSet1 = view1->dataSet.get();
      dataSet2 = view2->dataSet.get();
      break;
  }
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
    offset = view->rawData()[1].size();
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

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::innerJoin() {
  switch (mode) {
    case JoinMode::DataSetDataSet:return innerJoin_DataSetDataSet();
    case JoinMode::ViewDataSet:return innerJoin_ViewDataSet();
    case JoinMode::ViewView:return innerJoin_ViewView();
  }
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::innerJoin_DataSetDataSet() {
  auto result = prepareResultView();

  const gsl::index fieldIndex1 = t1->dataSet->fieldByName(col1)->getIndex();
  const auto type1 = t1->dataSet->fieldByName(col1)->getFieldType();
  const gsl::index fieldIndex2 = t2->dataSet->fieldByName(col2)->getIndex();

  auto cmpFunc = [&result]
      (int8_t cmpResult,
       bool _,
       DataSets::MemoryDataSet::iterator &iter1,
       DataSets::MemoryDataSet::iterator &iter2) {
    if (cmpResult == 0) {
      result->rawData()->emplace_back(std::vector<DataSetRow *>{
          *iter1, *iter2});
    }
  };

  iterAndCompare(t1->dataSet, t2->dataSet,
                 std::make_pair(fieldIndex1, 0),
                 std::make_pair(fieldIndex2, 0),
                 cmpFunc,
                 type1);

  result->rawData()->emplace_back();
  return result;
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::innerJoin_ViewDataSet() {
  auto result = prepareResultView();

  const gsl::index field1TableIndex =
      (view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskTableIndex) >> view1->dataSet->maskTableShift;
  const gsl::index field1FieldIndex =
      view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskColumnIndex;
  const auto type1 = view1->dataSet->fieldByName(col1)->getFieldType();

  const gsl::index fieldIndex2 = t2->dataSet->fieldByName(col2)->getIndex();

  auto cmpFunc = [&result]
      (int8_t cmpResult,
       bool _,
       DataSets::MemoryViewDataSet::iterator &iter1,
       DataSets::MemoryDataSet::iterator &iter2) {
    if (cmpResult == 0) {
      std::vector<DataSetRow *> newRecord;
      std::copy((*iter1).begin(),
                (*iter1).end(),
                std::back_inserter(newRecord));
      newRecord.emplace_back(*iter2);
      result->rawData()->emplace_back(newRecord);
    }
  };

  iterAndCompare(view1->dataSet, t2->dataSet,
                 std::make_pair(field1TableIndex, field1FieldIndex),
                 std::make_pair(fieldIndex2, 0),
                 cmpFunc,
                 type1);

  result->rawData()->emplace_back();
  return result;
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::innerJoin_ViewView() {
  auto result = prepareResultView();

  const gsl::index field1TableIndex =
      (view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskTableIndex) >> view1->dataSet->maskTableShift;
  const gsl::index field1FieldIndex =
      view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskColumnIndex;
  const auto type1 = view1->dataSet->fieldByName(col1)->getFieldType();

  const gsl::index field2TableIndex =
      (view2->dataSet->fieldByName(col2)->getIndex()
          & view2->dataSet->maskTableIndex) >> view2->dataSet->maskTableShift;
  const gsl::index field2FieldIndex =
      view2->dataSet->fieldByName(col2)->getIndex()
          & view2->dataSet->maskColumnIndex;

  auto cmpFunc = [&result]
      (int8_t cmpResult,
       bool _,
       DataSets::MemoryViewDataSet::iterator &iter1,
       DataSets::MemoryViewDataSet::iterator &iter2) {
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

  iterAndCompare(view1->dataSet, view2->dataSet,
                 std::make_pair(field1TableIndex, field1FieldIndex),
                 std::make_pair(field2TableIndex, field2FieldIndex),
                 cmpFunc,
                 type1);

  result->rawData()->emplace_back();
  return result;
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::leftJoin() {
  switch (mode) {
    case JoinMode::DataSetDataSet:return leftJoin_DataSetDataSet();
    case JoinMode::ViewDataSet:return leftJoin_ViewDataSet();
    case JoinMode::ViewView:return leftJoin_ViewView();
  }
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::leftJoin_DataSetDataSet() {
  auto result = prepareResultView();

  const gsl::index fieldIndex1 = t1->dataSet->fieldByName(col1)->getIndex();
  const auto type1 = t1->dataSet->fieldByName(col1)->getFieldType();

  const gsl::index fieldIndex2 = t2->dataSet->fieldByName(col2)->getIndex();

  auto cmpFunc = [&result]
      (int8_t cmpResult,
       bool found,
       DataSets::MemoryDataSet::iterator &iter1,
       DataSets::MemoryDataSet::iterator &iter2) {
    if (cmpResult == 0) {
      result->rawData()->emplace_back(std::vector<DataSetRow *>{
          *iter1, *iter2});
    } else if (cmpResult == -1 && !found) {
      result->rawData()->emplace_back(std::vector<DataSetRow *>{
          *iter1, result->getNullRow(1)});
    }
  };

  iterAndCompare(t1->dataSet, t2->dataSet,
                 std::make_pair(fieldIndex1, 0),
                 std::make_pair(fieldIndex2, 0),
                 cmpFunc,
                 type1);

  result->rawData()->emplace_back();
  return result;
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::leftJoin_ViewDataSet() {
  auto result = prepareResultView();

  const gsl::index field1TableIndex =
      (view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskTableIndex) >> view1->dataSet->maskTableShift;
  const gsl::index field1FieldIndex =
      view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskColumnIndex;
  const auto type1 = view1->dataSet->fieldByName(col1)->getFieldType();

  const gsl::index fieldIndex2 = t2->dataSet->fieldByName(col2)->getIndex();

  auto cmpFunc = [&result]
      (int8_t cmpResult,
       bool found,
       DataSets::MemoryViewDataSet::iterator &iter1,
       DataSets::MemoryDataSet::iterator &iter2) {
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
      newRecord.emplace_back(result->getNullRow(1));
      result->rawData()->emplace_back(newRecord);
    }
  };

  iterAndCompare(view1->dataSet, t2->dataSet,
                 std::make_pair(field1TableIndex, field1FieldIndex),
                 std::make_pair(fieldIndex2, 0),
                 cmpFunc,
                 type1);

  result->rawData()->emplace_back();
  return result;
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::leftJoin_ViewView() {
  auto result = prepareResultView();

  const gsl::index field1TableIndex =
      (view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskTableIndex) >> view1->dataSet->maskTableShift;
  const gsl::index field1FieldIndex =
      view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskColumnIndex;
  const auto type1 = view1->dataSet->fieldByName(col1)->getFieldType();

  const gsl::index field2TableIndex =
      (view2->dataSet->fieldByName(col2)->getIndex()
          & view2->dataSet->maskTableIndex) >> view2->dataSet->maskTableShift;
  const gsl::index field2FieldIndex =
      view2->dataSet->fieldByName(col2)->getIndex()
          & view2->dataSet->maskColumnIndex;

  auto cmpFunc = [&result]
      (int8_t cmpResult,
       bool found,
       DataSets::MemoryViewDataSet::iterator &iter1,
       DataSets::MemoryViewDataSet::iterator &iter2) {
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
      newRecord.emplace_back(result->getNullRow(1));
      result->rawData()->emplace_back(newRecord);
    }
  };

  iterAndCompare(view1->dataSet, view2->dataSet,
                 std::make_pair(field1TableIndex, field1FieldIndex),
                 std::make_pair(field2TableIndex, field2FieldIndex),
                 cmpFunc,
                 type1);

  result->rawData()->emplace_back();
  return result;
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::rightJoin_DataSetDataSet() {
  throw NotImplementedException();
}
std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::rightJoin_ViewDataSet() {
  throw NotImplementedException();
}
std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::rightJoin_ViewView() {
  throw NotImplementedException();
}
std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::outerJoin_DataSetDataSet() {
  throw NotImplementedException();
}
std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::outerJoin_ViewDataSet() {
  throw NotImplementedException();
}
std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::outerJoin_ViewView() {
  throw NotImplementedException();
}

template<typename T, typename T2, typename CompFunc>
void DataBase::JoinMaker::iterAndCompare(std::shared_ptr<T> first,
                                         std::shared_ptr<T2> second,
                                         std::pair<gsl::index,
                                                   gsl::index> firstIndex,
                                         std::pair<gsl::index,
                                                   gsl::index> secondIndex,
                                         CompFunc cmpFunc,
                                         ValueType valueType) {
  static_assert(std::is_same<T, DataSets::MemoryDataSet>{}
                    || std::is_same<T, DataSets::MemoryViewDataSet>{});
  static_assert(std::is_same<T2, DataSets::MemoryDataSet>{}
                    || std::is_same<T2, DataSets::MemoryViewDataSet>{});

  auto begin1 = first->begin();
  auto end1 = first->end();
  auto begin2 = second->begin();
  auto end2 = second->end();

  DataContainer container1, container2;
  bool found = false;
  int diff = 0;
  while (begin1 != end1) {
    while (begin2 != end2) {
      if constexpr(std::is_same<T, DataSets::MemoryViewDataSet>{}) {
        container1 = (*(*begin1)[firstIndex.first])[firstIndex.second];
      } else if constexpr(std::is_same<T, DataSets::MemoryDataSet>{}) {
        container1 = (*(*begin1))[firstIndex.first];
      }

      if constexpr(std::is_same<T2, DataSets::MemoryViewDataSet>{}) {
        container2 = (*(*begin2)[secondIndex.first])[secondIndex.second];
      } else if constexpr(std::is_same<T2, DataSets::MemoryDataSet>{}) {
        container2 = (*(*begin2))[secondIndex.first];
      }

      auto cmpResult = compareDataContainers(container1, container2, valueType);

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

