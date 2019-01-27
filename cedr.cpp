

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
const std::string prijemceCSVName = "prijemcePomoci.csv";
const std::string outputCSVName = "cedr_output.csv";
const std::string outputCSVAgrName = "cedr_output_agr.csv";
const std::string subjektyCSVName = "NNO_subjekty.csv";

const std::string QUERY = "SELECT main.idDotace, main.idPrijemce, main.projektNazev, main.iriOperacniProgram, "
                          "main.iriGrantoveSchema, main.idRozhodnuti, main.castkaCerpana, main.castkaUvolnena, "
                          "main.iriDotacniTitul, main.iriPoskytovatelDotace, main.rozpoctoveObdobi, "
                          "prijemce_subjekty.idPrijemce, prijemce_subjekty.ico, prijemce_subjekty.obchodniJmeno, "
                          "prijemce_subjekty.obchodniForma, "
                          "prijemce_subjekty.Id_GIS, prijemce_subjekty.SouradniceA, "
                          "prijemce_subjekty.SouradniceB, prijemce_subjekty.X, prijemce_subjekty.Y "
                          "FROM main JOIN prijemce_subjekty ON main.idPrijemce = prijemce_subjekty.idPrijemce "
                          "WHERE main.rozpoctoveObdobi = 2017";

const std::string QUERY_AGR = "SELECT main.idPrijemce, SUM(main.castkaCerpana), SUM(main.castkaUvolnena), "
                              "main.rozpoctoveObdobi, "
                              "prijemce_subjekty.idPrijemce, prijemce_subjekty.ico, prijemce_subjekty.obchodniJmeno, "
                              "prijemce_subjekty.jmeno, prijemce_subjekty.prijmeni, prijemce_subjekty.rokNarozeni, "
                              "prijemce_subjekty.Id_GIS, prijemce_subjekty.SouradniceA, "
                              "prijemce_subjekty.SouradniceB, prijemce_subjekty.X, prijemce_subjekty.Y "
                              "FROM main JOIN prijemce_subjekty ON main.idPrijemce = prijemce_subjekty.idPrijemce "
                              "WHERE main.rozpoctoveObdobi = 2017";

void countIntersection() {
  auto prijemceProvider = new DataProviders::CsvReader(csvPath + prijemceCSVName, ",");
  auto subjektyProvider = new DataProviders::CsvReader(csvPath + subjektyCSVName, ";");

  auto prijemceDataSet = new DataSets::MemoryDataSet("prijemce");
  prijemceDataSet->setDataProvider(prijemceProvider);
  prijemceDataSet->setFieldTypes({STRING_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL,
                                  STRING_VAL});
  prijemceDataSet->open();
  Logger::log(Debug, "Prijemce loaded", true);

  auto subjektyDataSet = new DataSets::MemoryDataSet("subjekty");
  subjektyDataSet->setDataProvider(subjektyProvider);
  subjektyDataSet->setFieldTypes({INTEGER_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL});
  subjektyDataSet->open();
  Logger::log(Debug, "Subjekty loaded", true);

  DataSets::SortOptions options;
  options.addOption(prijemceDataSet->fieldByName("ico")->getIndex(), ASCENDING);
  prijemceDataSet->sort(options);

  DataSets::SortOptions options2;
  options2.addOption(subjektyDataSet->fieldByName("ICO_num")->getIndex(), ASCENDING);
  subjektyDataSet->sort(options2);

  Logger::log(Debug, "Sorted", true);

  auto fieldPrijemce = dynamic_cast<DataSets::IntegerField *>(prijemceDataSet->fieldByName("ico"));
  auto fieldSubjekty = dynamic_cast<DataSets::IntegerField *>(subjektyDataSet->fieldByName("ICO_num"));
  int cnt = 0;
  while (!prijemceDataSet->eof()) {

    while (!subjektyDataSet->eof()) {

      switch (Utilities::compareInt(fieldPrijemce->getAsInteger(), fieldSubjekty->getAsInteger())) {
        case 0:cnt++;
          goto man;
        case -1:goto man;
        case 1:goto uganda;
      }
      uganda:
      subjektyDataSet->next();
    }
    man:

    if (subjektyDataSet->eof()) {
      break;
    }

    prijemceDataSet->next();
  }
  Logger::log(Info, "Total intersection count: " + std::to_string(cnt), true);

}

int main(int argc, char **argv) {
  //countIntersection();
  //return 0;
  // open input files
  auto dotaceProvider = new DataProviders::CsvReader(csvPath + dotaceCSVName, ",");
  auto rozhodnutiProvider = new DataProviders::CsvReader(csvPath + rozhodnutiCSVName, ",");
  auto obdobiProvider = new DataProviders::CsvReader(csvPath + obdobiCSVName, ",");
  auto prijemceProvider = new DataProviders::CsvReader(csvPath + prijemceCSVName, ",");
  auto subjektyProvider = new DataProviders::CsvReader(csvPath + subjektyCSVName, ";");

  Logger::log(Debug, "Providers prepared", true);

  auto dotaceDataSet = new DataSets::MemoryDataSet("dotace");
  dotaceDataSet->setDataProvider(dotaceProvider);
  dotaceDataSet->setFieldTypes({STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL});
  dotaceDataSet->open();
  Logger::log(Debug, "Dotace loaded", true);

  auto rozhodnutiDataSet = new DataSets::MemoryDataSet("rozhodnuti");
  rozhodnutiDataSet->setDataProvider(rozhodnutiProvider);
  rozhodnutiDataSet->setFieldTypes({STRING_VAL, STRING_VAL, STRING_VAL, INTEGER_VAL});
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

  merger.removeDataSet("dotace_rozhodnuti");
  merger.removeDataSet("obdobi");
  delete obdobiDataSet;
  delete dotace_rozhodnutiDataSet;

  auto dataWorker = new DataWorkers::MemoryDataWorker(dotace_rozhodnuti_obdobiDataSet);

  auto dataWriter = new CsvWriter(csvPath + outputCSVName);

  auto prijemceDataSet = new DataSets::MemoryDataSet("prijemce");
  prijemceDataSet->setDataProvider(prijemceProvider);
  prijemceDataSet->setFieldTypes({STRING_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL,
                                  STRING_VAL});
  prijemceDataSet->open();
  Logger::log(Debug, "Prijemce loaded", true);

  auto subjektyDataSet = new DataSets::MemoryDataSet("subjekty");
  subjektyDataSet->setDataProvider(subjektyProvider);
  subjektyDataSet->setFieldTypes({INTEGER_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL});
  subjektyDataSet->open();
  Logger::log(Debug, "Subjekty loaded", true);

  merger.addDataSet(subjektyDataSet);
  merger.addDataSet(prijemceDataSet);
  auto prijemce_subjektyDataSet = merger.mergeDataSets("prijemce", "subjekty", "ico", "ICO_num");

  delete subjektyDataSet;
  delete prijemceDataSet;

  dataWorker->addDataSet(prijemce_subjektyDataSet);

  std::string query = QUERY;
  dataWorker->writeResult(*dataWriter,
                          query);

  Logger::log(Debug, "Written query: " + QUERY, true);

  delete dataWriter;
  dataWriter = new CsvWriter(csvPath + outputCSVAgrName);
  query = QUERY_AGR;
  dataWorker->writeResult(*dataWriter,
                          query);
  Logger::log(Debug, "Written query: " + QUERY_AGR, true);

  delete dataWorker;
  delete subjektyProvider;
  Logger::log(Debug, "Done", true);
  return 0;
}
