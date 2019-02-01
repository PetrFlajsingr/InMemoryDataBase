//
// Created by Petr Flajsingr on 12/11/2018.
//

#ifndef CSV_READER_FINMDATAWORKER_H
#define CSV_READER_FINMDATAWORKER_H

#include "BaseDataWorker.h"
#include "Exceptions.h"
#include "BaseField.h"
#include "IntegerField.h"
#include "DoubleField.h"
#include "CurrencyField.h"
#include "StringField.h"
#include "SortStructures.h"

namespace DataWorkers {

class ResultAccumulator {
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

 public:
  ResultAccumulator(DataSets::BaseField *field, Operation op);

  bool step();

  std::string getResult();

  std::string getResultForce();

  void reset();

  std::string getName();

  DataContainer getContainer();
};

class MemoryDataWorker : public BaseDataWorker {
 private:
  struct InnerJoinStructure {
    uint64_t indexAddi;
    uint64_t indexMain;

    std::vector<DataSets::BaseField*> projectFields;
  };

  void writeHeaders(DataWriters::BaseDataWriter &writer);

  void filterDataSet();

  void T_ThreadSort(DataSets::SortOptions &sortOptions);
 public:
  MemoryDataWorker(DataProviders::BaseDataProvider *dataProvider,
                   std::vector<ValueType> fieldTypes);

  MemoryDataWorker(DataSets::BaseDataSet *dataSet);

  std::vector<std::string> getMultiChoiceNames() override;

  std::vector<std::string> getChoices(std::string choiceName) override;

  void writeResult(DataWriters::BaseDataWriter &writer,
                   const std::string &sql) override;
};
}  // namespace DataWorkers

#endif //CSV_READER_FINMDATAWORKER_H
