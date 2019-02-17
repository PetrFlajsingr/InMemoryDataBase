//
// Created by Petr Flajsingr on 2019-02-08.
//

#ifndef PROJECT_JOINMAKER_H
#define PROJECT_JOINMAKER_H

#include <MemoryViewDataSet.h>
#include <MemoryDataBase.h>

namespace DataBase {
class JoinMaker {
 public:
  JoinMaker(const std::shared_ptr<Table> &t1,
            const std::string &field1,
            const std::shared_ptr<Table> &t2,
            const std::string &field2);
  JoinMaker(const std::shared_ptr<View> &view1,
            const std::string &field1,
            const std::shared_ptr<Table> &t2,
            const std::string &field2);
  JoinMaker(const std::shared_ptr<View> &view1,
            const std::string &field1,
            const std::shared_ptr<View> &view2,
            const std::string &field2);

  std::shared_ptr<DataSets::MemoryViewDataSet> innerJoin();

  std::shared_ptr<DataSets::MemoryViewDataSet> leftJoin();

  std::shared_ptr<DataSets::MemoryViewDataSet> rightJoin();

  std::shared_ptr<DataSets::MemoryViewDataSet> outerJoin();

 private:
  std::shared_ptr<DataSets::MemoryViewDataSet> innerJoin_DataSetDataSet();
  std::shared_ptr<DataSets::MemoryViewDataSet> innerJoin_ViewDataSet();
  std::shared_ptr<DataSets::MemoryViewDataSet> innerJoin_ViewView();

  std::shared_ptr<DataSets::MemoryViewDataSet> leftJoin_DataSetDataSet();
  std::shared_ptr<DataSets::MemoryViewDataSet> leftJoin_ViewDataSet();
  std::shared_ptr<DataSets::MemoryViewDataSet> leftJoin_ViewView();

  std::shared_ptr<DataSets::MemoryViewDataSet> rightJoin_DataSetDataSet();
  std::shared_ptr<DataSets::MemoryViewDataSet> rightJoin_ViewDataSet();
  std::shared_ptr<DataSets::MemoryViewDataSet> rightJoin_ViewView();

  std::shared_ptr<DataSets::MemoryViewDataSet> outerJoin_DataSetDataSet();
  std::shared_ptr<DataSets::MemoryViewDataSet> outerJoin_ViewDataSet();
  std::shared_ptr<DataSets::MemoryViewDataSet> outerJoin_ViewView();

  enum class JoinMode {
    DataSetDataSet, ViewDataSet, ViewView
  };

  std::shared_ptr<View> view1;
  std::shared_ptr<Table> t1;
  std::string col1;
  std::shared_ptr<View> view2;
  std::shared_ptr<Table> t2;
  std::string col2;
  JoinMode mode;

  std::shared_ptr<DataSets::MemoryViewDataSet> prepareResultView();
};
}

#endif //PROJECT_JOINMAKER_H
