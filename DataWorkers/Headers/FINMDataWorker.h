//
// Created by Petr Flajsingr on 12/11/2018.
//

#ifndef CSV_READER_FINMDATAWORKER_H
#define CSV_READER_FINMDATAWORKER_H

#include "BaseDataWorker.h"
#include "Exceptions.h"
#include "BaseField.h"

namespace DataWorkers {
class FINMDataWorker : public BaseDataWorker {
 public:
  FINMDataWorker(CsvWriter *writer,
      DataProviders::BaseDataProvider *dataProvider,
                 std::vector<ValueType> fieldTypes);

  std::string getMultiChoiceNames() override;

  std::vector<std::string> getChoices(std::string choiceName) override;

  void setFilters(std::vector<DataSets::FilterOptions> filters) override;
};
}  // namespace DataWorkers

#endif //CSV_READER_FINMDATAWORKER_H
