//
// Created by Petr Flajsingr on 12/11/2018.
//

#ifndef CSV_READER_FINMDATAWORKER_H
#define CSV_READER_FINMDATAWORKER_H

#include "BaseDataWorker.h"
#include "Exceptions.h"

namespace DataWorkers {
class FINMDataWorker : public BaseDataWorker {
 public:
  std::string getMultiChoiceNames() override;
  std::string getChoices(std::string choiceName) override;
  void setFilters(std::vector<DataSets::FilterOptions> filters) override;
};
}  // namespace DataWorkers

#endif //CSV_READER_FINMDATAWORKER_H
