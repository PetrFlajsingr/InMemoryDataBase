

#include <iostream>
#include <iomanip>
#include <MemoryDataSet.h>
#include <DataSetMerger.h>
#include <Logger.h>
#include <MemoryDataWorker.h>
#include "DataProviders/Headers/CsvReader.h"

const std::string csvPath = "/Users/petr/Desktop/csvs/";
const std::string dotaceCSVName = "dotace.csv";
const std::string rozhodnutiCSVName = "rozhodnuti.csv";
const std::string obdobiCSVName = "rozpoctoveObdobi.csv";
const std::string outputCSVName = "cedr_output.csv";

const std::string QUERY = "SELECT main.idDotace, main.idPrijemce, main.projektNazev, main.iriOperacniProgram, "
                          "main.iriGrantoveSchema, main.idRozhodnuti, main.castkaCerpana, main.castkaUvolnena, "
                          "main.iriDotacniTitul, main.iriPoskytovatelDotace, main.rokRozhodnuti FROM main "
                          "WHERE main.rokRozhodnuti = 2017";

const std::string QUERY_AGR = "SELECT main.idPrijemce, SUM(main.castkaCerpana), SUM(main.castkaUvolnena), "
                              "main.rokRozhodnuti FROM main "
                              "WHERE main.rokRozhodnuti = 2017";

int main(int argc, char **argv) {
  // open input files
  auto dotaceProvider = new DataProviders::CsvReader(csvPath + dotaceCSVName, ",");
  auto rozhodnutiProvider = new DataProviders::CsvReader(csvPath + rozhodnutiCSVName, ",");
  auto obdobiProvider = new DataProviders::CsvReader(csvPath + obdobiCSVName, ",");

  Logger::log(Debug, "Providers prepared", true);

  auto dotaceDataSet = new DataSets::MemoryDataSet("dotace");
  dotaceDataSet->setDataProvider(dotaceProvider);
  dotaceDataSet->setFieldTypes({STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL});
  dotaceDataSet->open();
  Logger::log(Debug, "Dotace loaded", true);

  auto rozhodnutiDataSet = new DataSets::MemoryDataSet("rozhodnuti");
  rozhodnutiDataSet->setDataProvider(rozhodnutiProvider);
  rozhodnutiDataSet->setFieldTypes({STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, INTEGER_VAL});
  rozhodnutiDataSet->open();
  Logger::log(Debug, "Rozhodnuti loaded", true);

  DataWorkers::DataSetMerger merger;
  merger.addDataSet(dotaceDataSet);
  merger.addDataSet(rozhodnutiDataSet);

  auto dotace_rozhodnutiDataSet = merger.mergeDataSets("dotace", "rozhodnuti", "idDotace", "idDotace");
  Logger::log(Debug, "Merged dotace, rozhodnuti", true);

  merger.removeDataSet("dotace");
  merger.removeDataSet("rozhodnuti");

  delete dotaceDataSet;
  delete rozhodnutiDataSet;

  auto obdobiDataSet = new DataSets::MemoryDataSet("obdobi");
  obdobiDataSet->setDataProvider(obdobiProvider);
  obdobiDataSet->setFieldTypes({STRING_VAL, STRING_VAL, CURRENCY_VAL, CURRENCY_VAL, STRING_VAL});
  obdobiDataSet->open();
  Logger::log(Debug, "Obdobi loaded", true);

  merger.addDataSet(dotace_rozhodnutiDataSet);
  merger.addDataSet(obdobiDataSet);

  auto dotace_rozhodnuti_obdobiDataSet =
      merger.mergeDataSets("dotace_rozhodnuti", "obdobi", "idRozhodnuti", "idRozhodnuti");
  Logger::log(Debug, "Merged dotace_rozhodnuti, obdobi", true);

  delete obdobiDataSet;
  delete dotace_rozhodnutiDataSet;

  auto dataWorker = new DataWorkers::MemoryDataWorker(dotace_rozhodnuti_obdobiDataSet);

  BaseDataWriter *dataWriter = new CsvWriter(csvPath + outputCSVName);

  std::string query = QUERY;
  dataWorker->writeResult(*dataWriter,
                          query);

  delete dotace_rozhodnuti_obdobiDataSet;
  Logger::log(Debug, "Done", true);
  return 0;
}
