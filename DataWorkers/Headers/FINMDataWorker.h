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

namespace DataWorkers {

class ResultAccumulator {
 private:
  union Data{
    int _int;
    double _double;
    char* _string;
    Currency* _currency;
  } data;

  uint32_t dataCount = 0;

  DataSets::BaseField* field;

  Operation operation;

  std::string result;

  bool handleDistinct();

  bool handleSum();

  bool handleAverage();

  std::string resultSum();

  std::string resultAverage();

 public:
  ResultAccumulator(DataSets::BaseField *field, Operation op);

  bool step();

  std::string getResult();

  void reset() {
    dataCount = 0;

    switch (field->getFieldType()) {
      case INTEGER_VAL:
        data._int = 0;
        break;
      case DOUBLE_VAL:
        data._double = 0;
        break;
      case CURRENCY:
        *data._currency = 0;
        break;
    }
  }

};


class FINMDataWorker : public BaseDataWorker {
 private:
  void writeHeaders(CsvWriter &writer);
 public:
  FINMDataWorker(DataProviders::BaseDataProvider *dataProvider,
                 std::vector<ValueType> fieldTypes);

  std::vector<std::string> getMultiChoiceNames() override;

  std::vector<std::string> getChoices(std::string choiceName) override;

  void filter(DataSets::FilterOptions &filters) override;

  void writeResult(CsvWriter &writer) override;
};
}  // namespace DataWorkers

#endif //CSV_READER_FINMDATAWORKER_H
