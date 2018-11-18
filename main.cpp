

#include <iostream>
#include <iomanip>
#include <sys/time.h>
#include "BaseDataSet.h"
#include "BaseField.h"
#include "MemoryDataSet.h"
#include "DataProviders/Headers/CsvReader.h"
#include "BaseDataWorker.h"
#include "FINMDataWorker.h"

#define PRINT(x) std::cout << x << std::endl;

const std::string FILEPATH = "/Users/petr/Desktop/MUNI/2010-2017/FINM201_2017012.csv";
const std::string DEST_FILEPATH = "/Users/Petr/Desktop/output.csv";
const std::string OBCEPATH = "/Users/petr/Desktop/MUNI/CSU_UIR_OBCE.csv";
const std::string KODYPATH = "/Users/petr/Desktop/MUNI/popis_kodu.csv";

std::string SQL = "SELECT main.ZC_ICO:ZC_ICO, main.0FUNC_AREA:0FUNC_AREA, SUM(main.ZU_ROZKZ:ZU_ROZKZ), obce.Obec, kody.Polozka_popis FROM main JOIN obce ON main.ZC_ICO:ZC_ICO = obce.ICO_num JOIN kody ON main.0FUNC_AREA:0FUNC_AREA = kody.Polozka_text";

void printTime(){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << std::endl;
}

long getMs() {
    struct timeval tp{};
    gettimeofday(&tp, nullptr);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int main(int argc, char** argv) {

  PRINT("create provider");
  printTime();
  DataProviders::BaseDataProvider* provider= new DataProviders::CsvReader(FILEPATH);
  PRINT("create provider done");
  printTime();

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
  PRINT("create worker");
  printTime();
  auto worker = new DataWorkers::FINMDataWorker(provider,
      fieldTypes);
  PRINT("create worker done");
  printTime();

  PRINT("create writer");
  printTime();
  BaseDataWriter *writer = new CsvWriter(DEST_FILEPATH);
  PRINT("create writer done");
  printTime();

  PRINT("create dataset obce");
  printTime();
  auto datasetObce = new DataSets::MemoryDataSet("obce");
  PRINT("create dataset obce done");
  printTime();

  PRINT("create provider obce");
  printTime();
  auto providerObce = new DataProviders::CsvReader(OBCEPATH);
  PRINT("create provider obce done");
  printTime();
  datasetObce->setDataProvider(providerObce);
  datasetObce->setFieldTypes({INTEGER_VAL, INTEGER_VAL, INTEGER_VAL, INTEGER_VAL, STRING_VAL,
                              INTEGER_VAL, STRING_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL,
                              STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, CURRENCY,
                              CURRENCY, CURRENCY, CURRENCY, CURRENCY, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL});
  PRINT("open dataset obce");
  printTime();
  datasetObce->open();
  PRINT("open dataset obce done");
  printTime();


  PRINT("open dataset kody");
  printTime();
  auto datasetKody = new DataSets::MemoryDataSet("kody");
  auto providerKody = new DataProviders::CsvReader(KODYPATH);
  datasetKody->setDataProvider(providerKody);
  datasetKody->setFieldTypes({INTEGER_VAL, STRING_VAL});
  datasetKody->open();
  PRINT("open dataset kody done");
  printTime();

  PRINT("add datasets done");
  printTime();
  worker->addDataSet(datasetObce);
  worker->addDataSet(datasetKody);
  PRINT("add datasets done");
  printTime();

  std::vector<DataWorkers::ProjectionOperation> ops;

  PRINT("write");
  printTime();
  worker->writeResult(*writer, SQL);
  PRINT("write done");
  printTime();

  delete writer;
  delete worker;

  return 0;
}
