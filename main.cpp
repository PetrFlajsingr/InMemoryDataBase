

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

//std::string SQL = "SELECT main.ZC_ICO:ZC_ICO, main.ZCMMT_ITM:ZCMMT_ITM, SUM(main.ZU_ROZKZ:ZU_ROZKZ), obce.Obec, kody.Polozka_popis FROM main JOIN obce ON main.ZC_ICO:ZC_ICO = obce.ICO_num JOIN kody ON main.ZCMMT_ITM:ZCMMT_ITM = kody.Polozka_text";

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

  auto worker = new DataWorkers::FINMDataWorker(provider,
      fieldTypes);


  BaseDataWriter *writer = new CsvWriter(DEST_FILEPATH);

  auto datasetObce = new DataSets::MemoryDataSet("obce");

  auto providerObce = new DataProviders::CsvReader(OBCEPATH);

  datasetObce->setDataProvider(providerObce);
  datasetObce->setFieldTypes({INTEGER_VAL, INTEGER_VAL, INTEGER_VAL, INTEGER_VAL, STRING_VAL,
                              INTEGER_VAL, STRING_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL,
                              STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, CURRENCY,
                              CURRENCY, CURRENCY, CURRENCY, CURRENCY, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL});
  datasetObce->open();

  auto datasetKody = new DataSets::MemoryDataSet("kody");
  auto providerKody = new DataProviders::CsvReader(KODYPATH);
  datasetKody->setDataProvider(providerKody);
  datasetKody->setFieldTypes({INTEGER_VAL, STRING_VAL});
  datasetKody->open();

  worker->addDataSet(datasetObce);
  worker->addDataSet(datasetKody);

  worker->writeResult(*writer, SQL);
  PRINT("STOP");
  printTime();
  PRINT("________");

  delete writer;
  delete worker;

  return 0;
}
