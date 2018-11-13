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

  bool handleDistinct() {
    switch (field->getFieldType()) {
      case INTEGER_VAL: {
        int value = reinterpret_cast<DataSets::IntegerField *>(field)
            ->getAsInteger();
        if (value != data._int) {
          result = std::to_string(data._int);
          data._int = value;
          return true;
        }
        break;
      }
      case DOUBLE_VAL: {
        double value = reinterpret_cast<DataSets::DoubleField *>(field)
            ->getAsDouble();
        if (value != data._double) {
          result = std::to_string(data._double);
          data._double = value;
          return true;
        }
        break;
      }
      case STRING_VAL: {
        std::string value = field->getAsString();
        if (value != data._string) {
          result = data._string;
          delete[] data._string;
          data._string = strdup(value.c_str());
          return true;
        }
        break;
      }
      case CURRENCY: {
        Currency value = reinterpret_cast<DataSets::CurrencyField *>(field)
            ->getAsCurrency();
        if (value != *data._currency) {
          result = dec::toString(*data._currency);
          *data._currency = value;
          return true;
        }
        break;
      }
    }
    return false;
  }

  bool handleSum() {
    switch (field->getFieldType()) {
      case INTEGER_VAL: {
        int value = reinterpret_cast<DataSets::IntegerField *>(field)
            ->getAsInteger();
        data._int += value;
        break;
      }
      case DOUBLE_VAL: {
        double value = reinterpret_cast<DataSets::DoubleField *>(field)
            ->getAsDouble();
        data._double += value;
        break;
      }
      case STRING_VAL: {
        throw IllegalStateException("Internal error.");
      }
      case CURRENCY: {
        Currency value = reinterpret_cast<DataSets::CurrencyField *>(field)
            ->getAsCurrency();
        *data._currency += value;
        break;
      }
    }
    return false;
  }

  bool handleAverage() {
    dataCount++;
    switch (field->getFieldType()) {
      case INTEGER_VAL: {
        int value = reinterpret_cast<DataSets::IntegerField *>(field)
            ->getAsInteger();
        data._int += value;
        break;
      }
      case DOUBLE_VAL: {
        double value = reinterpret_cast<DataSets::DoubleField *>(field)
            ->getAsDouble();
        data._double += value;
        break;
      }
      case STRING_VAL: {
        throw IllegalStateException("Internal error.");
      }
      case CURRENCY: {
        Currency value = reinterpret_cast<DataSets::CurrencyField *>(field)
            ->getAsCurrency();
        *data._currency += value;
        break;
      }
    }
    return false;
  }

  std::string resultSum() {
    switch (field->getFieldType()) {
      case INTEGER_VAL: {
        return std::to_string(data._int);
      }
      case DOUBLE_VAL: {
        return std::to_string(data._double);
      }
      case CURRENCY: {
        return dec::toString(*data._currency);
      }
    }
  }

  std::string resultAverage() {
    switch (field->getFieldType()) {
      case INTEGER_VAL: {
        return std::to_string(data._int / dataCount);
      }
      case DOUBLE_VAL: {
        return std::to_string(data._double / dataCount);
      }
      case CURRENCY: {
        return dec::toString(*data._currency / Currency(dataCount));
      }
    }
  }

 public:
  ResultAccumulator(DataSets::BaseField *field, Operation op);

  bool step() {
    switch (operation) {
      case Distinct:
        return handleDistinct();
      case Sum:
        return handleSum();
      case Average:
        return handleAverage();
      default:
        throw IllegalStateException("Internal error.");
    }
  }

  std::string getResult() {
    switch (operation) {
      case Distinct:
        return result;
      case Sum:
        return resultSum();
      case Average:
        return resultAverage();
    }
    throw IllegalStateException("Internal error.");
  }

};


class FINMDataWorker : public BaseDataWorker {
 private:
  //TODO
  const std::vector<std::string> columnChoices = {"aa"};

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
