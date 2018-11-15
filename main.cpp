

#include <iostream>
#include <iomanip>
#include <sys/time.h>
#include "BaseDataSet.h"
#include "BaseField.h"
#include "MemoryDataSet.h"
#include "DataProviders/Headers/CsvReader.h"
#include "BaseDataWorker.h"
#include "FINMDataWorker.h"

const std::string FILEPATH = "C:\\Users\\dvorak\\OneDrive - MUNI\\FINM201_2017012.csv";
const std::string DEST_FILEPATH = "C:\\Users\\dvorak\\OneDrive - MUNI\\output";

void printTime(){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
   // std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << std::endl;
}

long getMs() {
    struct timeval tp{};
    gettimeofday(&tp, nullptr);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int main(int argc, char** argv) {
  DataProviders::BaseDataProvider* provider= new DataProviders::CsvReader(FILEPATH);

  std::vector<ValueType> fieldTypes = {
      INTEGER_VAL,
      INTEGER_VAL,
      INTEGER_VAL,
      INTEGER_VAL,
      INTEGER_VAL,
      STRING_VAL,
      STRING_VAL,
      INTEGER_VAL,
      INTEGER_VAL,
      INTEGER_VAL,
      CURRENCY,
      CURRENCY,
      CURRENCY
  };
  DataWorkers::BaseDataWorker* worker = new DataWorkers::FINMDataWorker(provider,
      fieldTypes);

  BaseDataWriter *writer = new CsvWriter(DEST_FILEPATH);

  std::vector<DataWorkers::ColumnOperation> ops;

  //ops.push_back({"ZC_UCJED:ZC_UCJED", DataWorkers::Distinct});
  ops.push_back({"ZC_ICO:ZC_ICO", DataWorkers::Distinct});
  ops.push_back({"ZCMMT_ITM:ZCMMT_ITM", DataWorkers::Distinct});
  //ops.push_back({"ZU_ROZSCH:ZU_ROZSCH", DataWorkers::Sum});
  //ops.push_back({"ZU_ROZPZM:ZU_ROZPZM", DataWorkers::Sum});
  ops.push_back({"ZU_ROZKZ:ZU_ROZKZ", DataWorkers::Sum});


  worker->setColumnOperations(ops);

  worker->writeResult(*writer);

  delete writer;
  delete worker;

  return 0;
}