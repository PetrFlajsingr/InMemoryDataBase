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
 private:
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
