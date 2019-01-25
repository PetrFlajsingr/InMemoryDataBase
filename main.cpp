

#include <iostream>
#include <iomanip>
#include <sys/time.h>
#include "BaseDataSet.h"
#include "BaseField.h"
#include "MemoryDataSet.h"
#include "DataProviders/Headers/CsvReader.h"
#include "BaseDataWorker.h"
#include "MemoryDataWorker.h"

#define PRINT(x) std::cout << x << std::endl;

const std::string FILEPATH = "/Users/petr/Desktop/MUNI/2010-2017/FINM201_2017012.csv";
const std::string DEST_DRUHOVE_FILEPATH = "/Users/Petr/Desktop/output_druhove.csv";
const std::string DEST_ODVETVOVE_FILEPATH = "/Users/Petr/Desktop/output_odvetvove.csv";
const std::string OBCEPATH = "/Users/petr/Desktop/MUNI/CSU_UIR_OBCE.csv";
const std::string KODY_DRUHOVE_PATH = "/Users/petr/Desktop/MUNI/popis_kodu_druhove.csv";
const std::string KODY_ODVETVOVE_PATH = "/Users/petr/Desktop/MUNI/popis_kodu_odvetvove.csv";

std::string SQLODVETVOVE = "SELECT main.ZC_ICO:ZC_ICO, main.0FUNC_AREA:0FUNC_AREA, SUM(main.ZU_ROZKZ:ZU_ROZKZ), obce.Obec, kody.Par_popis FROM main JOIN obce ON main.ZC_ICO:ZC_ICO = obce.ICO_num JOIN kody ON main.0FUNC_AREA:0FUNC_AREA = kody.Par_num";

std::string SQLDRUHOVE = "SELECT main.ZC_ICO:ZC_ICO, main.ZCMMT_ITM:ZCMMT_ITM, SUM(main.ZU_ROZKZ:ZU_ROZKZ), obce.Obec, kody.Polozka_popis FROM main JOIN obce ON main.ZC_ICO:ZC_ICO = obce.ICO_num JOIN kody ON main.ZCMMT_ITM:ZCMMT_ITM = kody.Polozka_text";

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

  PRINT("START");
  printTime();
  PRINT("________");
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

  auto worker = new DataWorkers::MemoryDataWorker(provider,
                                                  fieldTypes);


  BaseDataWriter *writer = new CsvWriter(DEST_DRUHOVE_FILEPATH);

  auto datasetObce = new DataSets::MemoryDataSet("obce");

  auto providerObce = new DataProviders::CsvReader(OBCEPATH);

  datasetObce->setDataProvider(providerObce);
  datasetObce->setFieldTypes({INTEGER_VAL, INTEGER_VAL, INTEGER_VAL, INTEGER_VAL, STRING_VAL,
                              INTEGER_VAL, STRING_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL,
                              STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, CURRENCY,
                              CURRENCY, CURRENCY, CURRENCY, CURRENCY, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL});
  datasetObce->open();

  auto datasetKodyDruhove = new DataSets::MemoryDataSet("kody");
  auto providerKody = new DataProviders::CsvReader(KODY_DRUHOVE_PATH);
  datasetKodyDruhove->setDataProvider(providerKody);
  datasetKodyDruhove->setFieldTypes({INTEGER_VAL, STRING_VAL});
  datasetKodyDruhove->open();

  worker->addDataSet(datasetObce);
  worker->addDataSet(datasetKodyDruhove);

  worker->writeResult(*writer, SQLDRUHOVE);
  delete writer;

  auto datasetKodyOdvetvove = new DataSets::MemoryDataSet("kody");
  auto providerKodyOdvetvove = new DataProviders::CsvReader(KODY_ODVETVOVE_PATH);
  datasetKodyOdvetvove->setDataProvider(providerKodyOdvetvove);
  datasetKodyOdvetvove->setFieldTypes({INTEGER_VAL, STRING_VAL});
  datasetKodyOdvetvove->open();

  writer = new CsvWriter(DEST_ODVETVOVE_FILEPATH);
  worker->clearDataSets();
  worker->addDataSet(datasetObce);
  worker->addDataSet(datasetKodyOdvetvove);

  worker->writeResult(*writer, SQLODVETVOVE);

  PRINT("STOP");
  printTime();
  PRINT("________");


  delete worker;

  return 0;
}
