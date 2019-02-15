//
// Created by Petr Flajsingr on 12/11/2018.
//

#ifndef CSV_READER_FINMDATAWORKER_H
#define CSV_READER_FINMDATAWORKER_H

#include <memory>
#include <BaseDataWorker.h>
#include <Exceptions.h>
#include <BaseField.h>
#include <IntegerField.h>
#include <DoubleField.h>
#include <CurrencyField.h>
#include <StringField.h>
#include <SortStructures.h>

namespace DataWorkers {

class ResultAccumulator {
 public:
  ResultAccumulator(DataSets::BaseField *field, Operation op);

  bool step();

  std::string getResult();

  std::string getResultForce();

  void reset();

  std::string_view getName();

  DataContainer getContainer();

 private:
  DataContainer data;

  DataContainer previousData;

  bool firstDone = false;

  bool distinct = false;

  uint32_t dataCount = 0;

  DataSets::BaseField* field;

  Operation operation;

  std::string result;

  bool handleDistinct();

  bool handleSum();

  bool handleAverage();

  std::string resultSum();

  std::string resultAverage();

  std::string resultDistinct();
};

class MemoryDataWorker : public BaseDataWorker {
 public:
  MemoryDataWorker(DataProviders::BaseDataProvider &dataProvider,
                   const std::vector<ValueType> &fieldTypes);

  explicit MemoryDataWorker(std::shared_ptr<DataSets::BaseDataSet> dataSet);

  std::vector<std::string> getMultiChoiceNames() override;

  std::vector<std::string> getChoices(std::string choiceName) override;

  void writeResult(DataWriters::BaseDataWriter &writer,
                   std::string_view sql) override;

 private:
  struct InnerJoinStructure {
    DataSets::BaseField *fieldAddi;
    DataSets::BaseField *fieldMain;

    std::vector<DataSets::BaseField*> projectFields;
  };

  void writeHeaders(DataWriters::BaseDataWriter &writer);

  std::shared_ptr<DataSets::ViewDataSet> filterDataSet();

  void T_ThreadSort(DataSets::SortOptions &sortOptions);
};
}  // namespace DataWorkers

#endif //CSV_READER_FINMDATAWORKER_H
