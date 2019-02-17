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
                [&fieldNames, &fieldTypes, &fieldIndices](const DataSets::BaseField *field) {
                  fieldNames.emplace_back(field->getName());
                  fieldTypes.emplace_back(field->getFieldType());
                  fieldIndices.emplace_back(1, field->getIndex());
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

  auto iter1 = t1->dataSet->begin();
  const gsl::index fieldIndex1 = t1->dataSet->fieldByName(col1)->getIndex();
  const auto type1 = t1->dataSet->fieldByName(col1)->getFieldType();

  auto iter2 = t2->dataSet->begin();
  const gsl::index fieldIndex2 = t2->dataSet->fieldByName(col2)->getIndex();

  bool found = false;
  int diff = 0;
  while (iter1 != t1->dataSet->end()) {
    while (iter2 != t2->dataSet->end()) {
      auto cmpResult = compareDataContainers(iter1->cells[fieldIndex1],
                                             iter2->cells[fieldIndex2],
                                             type1);
      switch (cmpResult) {
        case 0: {
          result->rawData()->emplace_back(std::vector<DataSets::DataSetRow *>{
              *iter1, *iter2});
          found = true;
          ++diff;
          break;
        }
        case -1: goto outer_loop;
      }

      ++iter2;
    }
    outer_loop:
    ++iter1;
    if (found) {
      found = false;
      iter2 -= diff;
      diff = 0;
    }
  }
  result->rawData()->emplace_back();
  return result;
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::innerJoin_ViewDataSet() {
  auto result = prepareResultView();

  auto iter1 = view1->dataSet->begin();
  const gsl::index field1TableIndex =
      (view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskTableIndex) >> view1->dataSet->maskTableShift;
  const gsl::index field1FieldIndex =
      view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskColumnIndex;
  const auto type1 = view1->dataSet->fieldByName(col1)->getFieldType();

  auto iter2 = t2->dataSet->begin();
  const gsl::index fieldIndex2 = t2->dataSet->fieldByName(col2)->getIndex();

  bool found = false;
  int diff = 0;
  while (iter1 != view1->dataSet->end()) {
    while (iter2 != t2->dataSet->end()) {
      auto cmpResult =
          compareDataContainers((*iter1)[field1TableIndex]->cells[field1FieldIndex],
                                iter2->cells[fieldIndex2],
                                type1);
      switch (cmpResult) {
        case 0: {
          std::vector<DataSets::DataSetRow *> newRecord;
          std::copy((*iter1).begin(),
                    (*iter1).end(),
                    std::back_inserter(newRecord));
          newRecord.emplace_back(*iter2);
          result->rawData()->emplace_back(newRecord);
          found = true;
          ++diff;
          break;
        }
        case -1: goto outer_loop;
      }

      ++iter2;
    }
    outer_loop:
    ++iter1;
    if (found) {
      found = false;
      iter2 -= diff;
    }
  }
  result->rawData()->emplace_back();
  return result;
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::innerJoin_ViewView() {
  auto result = prepareResultView();

  auto iter1 = view1->dataSet->begin();
  const gsl::index field1TableIndex =
      (view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskTableIndex) >> view1->dataSet->maskTableShift;
  const gsl::index field1FieldIndex =
      view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskColumnIndex;
  const auto type1 = view1->dataSet->fieldByName(col1)->getFieldType();

  auto iter2 = view2->dataSet->begin();
  const gsl::index field2TableIndex =
      (view2->dataSet->fieldByName(col2)->getIndex()
          & view2->dataSet->maskTableIndex) >> view2->dataSet->maskTableShift;
  const gsl::index field2FieldIndex =
      view2->dataSet->fieldByName(col2)->getIndex()
          & view2->dataSet->maskColumnIndex;

  bool found = false;
  int diff = 0;
  while (iter1 != view1->dataSet->end()) {
    while (iter2 != view2->dataSet->end()) {
      auto cmpResult =
          compareDataContainers((*iter1)[field1TableIndex]->cells[field1FieldIndex],
                                (*iter2)[field2TableIndex]->cells[field2FieldIndex],
                                type1);
      switch (cmpResult) {
        case 0: {
          std::vector<DataSets::DataSetRow *> newRecord;
          std::copy((*iter1).begin(),
                    (*iter1).end(),
                    std::back_inserter(newRecord));
          std::copy((*iter2).begin(),
                    (*iter2).end(),
                    std::back_inserter(newRecord));
          result->rawData()->emplace_back(newRecord);
          found = true;
          ++diff;
          break;
        }
        case -1: goto outer_loop;
      }

      ++iter2;
    }
    outer_loop:
    ++iter1;
    if (found) {
      found = false;
      iter2 -= diff;
    }
  }
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

  auto iter1 = t1->dataSet->begin();
  const gsl::index fieldIndex1 = t1->dataSet->fieldByName(col1)->getIndex();
  const auto type1 = t1->dataSet->fieldByName(col1)->getFieldType();

  auto iter2 = t2->dataSet->begin();
  const gsl::index fieldIndex2 = t2->dataSet->fieldByName(col2)->getIndex();

  bool found = false;
  int diff = 0;
  while (iter1 != t1->dataSet->end()) {
    while (iter2 != t2->dataSet->end()) {
      auto cmpResult = compareDataContainers(iter1->cells[fieldIndex1],
                                             iter2->cells[fieldIndex2],
                                             type1);
      switch (cmpResult) {
        case 0: {
          result->rawData()->emplace_back(std::vector<DataSets::DataSetRow *>{
              *iter1, *iter2});
          found = true;
          ++diff;
          break;
        }
        case -1:
          if (!found) {
            result->rawData()->emplace_back(std::vector<DataSets::DataSetRow *>{
                *iter1, result->getNullRow(1)});
          }
          goto outer_loop;
      }

      ++iter2;
    }
    outer_loop:
    ++iter1;
    if (found) {
      found = false;
      iter2 -= diff;
    }
  }
  result->rawData()->emplace_back();
  return result;
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::leftJoin_ViewDataSet() {
  auto result = prepareResultView();

  auto iter1 = view1->dataSet->begin();
  const gsl::index field1TableIndex =
      (view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskTableIndex) >> view1->dataSet->maskTableShift;
  const gsl::index field1FieldIndex =
      view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskColumnIndex;
  const auto type1 = view1->dataSet->fieldByName(col1)->getFieldType();

  auto iter2 = t2->dataSet->begin();
  const gsl::index fieldIndex2 = t2->dataSet->fieldByName(col2)->getIndex();

  bool found = false;
  int diff = 0;
  while (iter1 != view1->dataSet->end()) {
    while (iter2 != t2->dataSet->end()) {
      auto cmpResult =
          compareDataContainers((*iter1)[field1TableIndex]->cells[field1FieldIndex],
                                iter2->cells[fieldIndex2],
                                type1);
      switch (cmpResult) {
        case 0: {
          std::vector<DataSets::DataSetRow *> newRecord;
          std::copy((*iter1).begin(),
                    (*iter1).end(),
                    std::back_inserter(newRecord));
          newRecord.emplace_back(*iter2);
          result->rawData()->emplace_back(newRecord);
          found = true;
          ++diff;
          break;
        }
        case -1:
          if (!found) {
            std::vector<DataSets::DataSetRow *> newRecord;
            std::copy((*iter1).begin(),
                      (*iter1).end(),
                      std::back_inserter(newRecord));
            newRecord.emplace_back(result->getNullRow(1));
            result->rawData()->emplace_back(newRecord);
          }
          goto outer_loop;
      }

      ++iter2;
    }
    outer_loop:
    ++iter1;
    if (found) {
      found = false;
      iter2 -= diff;
    }
  }
  result->rawData()->emplace_back();
  return result;
}

std::shared_ptr<DataSets::MemoryViewDataSet> DataBase::JoinMaker::leftJoin_ViewView() {
  auto result = prepareResultView();

  auto iter1 = view1->dataSet->begin();
  const gsl::index field1TableIndex =
      (view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskTableIndex) >> view1->dataSet->maskTableShift;
  const gsl::index field1FieldIndex =
      view1->dataSet->fieldByName(col1)->getIndex()
          & view1->dataSet->maskColumnIndex;
  const auto type1 = view1->dataSet->fieldByName(col1)->getFieldType();

  auto iter2 = view2->dataSet->begin();
  const gsl::index field2TableIndex =
      (view2->dataSet->fieldByName(col2)->getIndex()
          & view2->dataSet->maskTableIndex) >> view2->dataSet->maskTableShift;
  const gsl::index field2FieldIndex =
      view2->dataSet->fieldByName(col2)->getIndex()
          & view2->dataSet->maskColumnIndex;

  bool found = false;
  int diff = 0;
  while (iter1 != view1->dataSet->end()) {
    while (iter2 != view2->dataSet->end()) {
      auto cmpResult =
          compareDataContainers((*iter1)[field1TableIndex]->cells[field1FieldIndex],
                                (*iter2)[field2TableIndex]->cells[field2FieldIndex],
                                type1);
      switch (cmpResult) {
        case 0: {
          std::vector<DataSets::DataSetRow *> newRecord;
          std::copy((*iter1).begin(),
                    (*iter1).end(),
                    std::back_inserter(newRecord));
          std::copy((*iter2).begin(),
                    (*iter2).end(),
                    std::back_inserter(newRecord));
          result->rawData()->emplace_back(newRecord);
          found = true;
          ++diff;
          break;
        }
        case -1:
          if (!found) {
            std::vector<DataSets::DataSetRow *> newRecord;
            std::copy((*iter1).begin(),
                      (*iter1).end(),
                      std::back_inserter(newRecord));
            newRecord.emplace_back(result->getNullRow(1));
            result->rawData()->emplace_back(newRecord);
          }
          goto outer_loop;
      }

      ++iter2;
    }
    outer_loop:
    ++iter1;
    if (found) {
      found = false;
      iter2 -= diff;
    }
  }
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
