

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
const std::string subjektyCSVName = "NNO_subjekty6ver2.3-2.csv";

const std::string operacniProgramCSVName = "operacniProgram.csv";
const std::string grantoveSchemaCSVName = "grantoveSchema.csv";
const std::string dotacniTitulCSVName = "dotaceTitul.csv";
const std::string poskytovatelDotaceCSVName = "poskytovatelDotace.csv";


const std::string QUERY = "SELECT main.idDotace, main.idPrijemce, main.projektNazev, main.iriOperacniProgram, "
                          "main.iriGrantoveSchema, main.idRozhodnuti, main.castkaCerpana, main.castkaUvolnena, "
                          "main.iriDotacniTitul, main.iriPoskytovatelDotace, main.rozpoctoveObdobi, "
                          "prijemce_subjekty.idPrijemce, prijemce_subjekty.ico, prijemce_subjekty.obchodniJmeno, "
                          "prijemce_subjekty.Id_GIS, prijemce_subjekty.SouřadniceA, "
                          "prijemce_subjekty.SouřadniceB, prijemce_subjekty.X, prijemce_subjekty.Y, "
                          "operacniProgram.operacniProgramNazev, "
                          "grantoveSchema.grantoveSchemaNazev, "
                          "dotaceTitul.dotaceTitulNazev "
                          "poskytovatelDotace.dotacePoskytovatelNazev, "
                          "FROM main JOIN prijemce_subjekty ON main.idPrijemce = prijemce_subjekty.idPrijemce "
                          "LEFT JOIN operacniProgram ON main.iriOperacniProgram = operacniProgram.idOperacniProgram "
                          "LEFT JOIN grantoveSchema ON main.iriGrantoveSchema = grantoveSchema.idGrantoveSchema "
                          "LEFT JOIN dotaceTitul ON main.iriDotacniTitul = dotaceTitul.idDotaceTitul "
                          "LEFT JOIN poskytovatelDotace ON main.iriPoskytovatelDotace = poskytovatelDotace.id";

const std::string QUERY_2017 = QUERY + " WHERE main.rozpoctoveObdobi = 2017";

const std::string QUERY_AGR = "SELECT main.idPrijemce, SUM(main.castkaCerpana), SUM(main.castkaUvolnena), "
                              "main.rozpoctoveObdobi, "
                              "prijemce_subjekty.idPrijemce, prijemce_subjekty.ico, prijemce_subjekty.obchodniJmeno, "
                              "prijemce_subjekty.Id_GIS, prijemce_subjekty.SouřadniceA, "
                              "prijemce_subjekty.SouřadniceB, prijemce_subjekty.X, prijemce_subjekty.Y, "
                              //"operacniProgram.operacniProgramNazev, "
                              //"grantoveSchema.grantoveSchemaNazev, "
                              //"dotaceTitul.dotaceTitulNazev, "
                              //"poskytovatelDotace.dotacePoskytovatelNazev "
                              "FROM main JOIN prijemce_subjekty ON main.idPrijemce = prijemce_subjekty.idPrijemce";
//"LEFT JOIN operacniProgram ON main.iriOperacniProgram = operacniProgram.idOperacniProgram "
//"LEFT JOIN grantoveSchema ON main.iriGrantoveSchema = grantoveSchema.idGrantoveSchema "
//"LEFT JOIN dotaceTitul ON main.iriDotacniTitul = dotaceTitul.idDotaceTitul "
//"LEFT JOIN poskytovatelDotace ON main.iriPoskytovatelDotace = poskytovatelDotace.id";

const std::string QUERY_AGR_2017 = QUERY_AGR + " WHERE main.rozpoctoveObdobi = 2017";

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
  options2.addOption(subjektyDataSet->fieldByName("IČO_num")->getIndex(), ASCENDING);
  subjektyDataSet->sort(options2);

  Logger::log(Debug, "Sorted", true);

  auto fieldPrijemce = dynamic_cast<DataSets::IntegerField *>(prijemceDataSet->fieldByName("ico"));
  auto fieldSubjekty = dynamic_cast<DataSets::IntegerField *>(subjektyDataSet->fieldByName("IČO_num"));
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
  rozhodnutiDataSet->setFieldTypes({STRING_VAL, STRING_VAL, STRING_VAL});
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
  obdobiDataSet->setFieldTypes({STRING_VAL, STRING_VAL, CURRENCY_VAL, CURRENCY_VAL, STRING_VAL, STRING_VAL});
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

  auto prijemceDataSet = new DataSets::MemoryDataSet("prijemce");
  prijemceDataSet->setDataProvider(prijemceProvider);
  prijemceDataSet->setFieldTypes({STRING_VAL, INTEGER_VAL, STRING_VAL});
  prijemceDataSet->open();
  Logger::log(Debug, "Prijemce loaded", true);

  auto subjektyDataSet = new DataSets::MemoryDataSet("subjekty");
  subjektyDataSet->setDataProvider(subjektyProvider);
  subjektyDataSet->setFieldTypes({INTEGER_VAL, INTEGER_VAL, INTEGER_VAL, STRING_VAL, STRING_VAL, STRING_VAL, STRING_VAL,
                                  STRING_VAL, STRING_VAL});
  subjektyDataSet->open();
  Logger::log(Debug, "Subjekty loaded", true);

  merger.addDataSet(subjektyDataSet);
  merger.addDataSet(prijemceDataSet);
  auto prijemce_subjektyDataSet = merger.mergeDataSets("prijemce", "subjekty", "ico", "IČO_num");

  Logger::log(Debug, "Merged prijemce, subjekty", true);

  delete subjektyDataSet;
  delete prijemceDataSet;

  auto operacniProgramProvider = new DataProviders::CsvReader(csvPath + operacniProgramCSVName, ",");
  auto operacniProgramDataSet = new DataSets::MemoryDataSet("operacniProgram");
  operacniProgramDataSet->setDataProvider(operacniProgramProvider);
  operacniProgramDataSet->setFieldTypes({STRING_VAL, STRING_VAL});
  operacniProgramDataSet->open();
  Logger::log(Debug, "operacniProgram loaded", true);

  auto grantoveSchemaProvider = new DataProviders::CsvReader(csvPath + grantoveSchemaCSVName, ",");
  auto grantoveSchemaDataSet = new DataSets::MemoryDataSet("grantoveSchema");
  grantoveSchemaDataSet->setDataProvider(grantoveSchemaProvider);
  grantoveSchemaDataSet->setFieldTypes({STRING_VAL, STRING_VAL});
  grantoveSchemaDataSet->open();
  Logger::log(Debug, "grantoveSchema loaded", true);

  auto dotaceTitulProvider = new DataProviders::CsvReader(csvPath + dotacniTitulCSVName, ",");
  auto dotaceTitulDataSet = new DataSets::MemoryDataSet("dotaceTitul");
  dotaceTitulDataSet->setDataProvider(dotaceTitulProvider);
  dotaceTitulDataSet->setFieldTypes({STRING_VAL, STRING_VAL});
  dotaceTitulDataSet->open();
  Logger::log(Debug, "dotaceTitul loaded", true);

  auto poskytovatelDotaceProvider = new DataProviders::CsvReader(csvPath + poskytovatelDotaceCSVName, ",");
  auto poskytovatelDotaceDataSet = new DataSets::MemoryDataSet("poskytovatelDotace");
  poskytovatelDotaceDataSet->setDataProvider(poskytovatelDotaceProvider);
  poskytovatelDotaceDataSet->setFieldTypes({STRING_VAL, STRING_VAL});
  poskytovatelDotaceDataSet->open();
  Logger::log(Debug, "poskytovatelDotace loaded", true);


  dataWorker->addDataSet(prijemce_subjektyDataSet);
  dataWorker->addDataSet(operacniProgramDataSet);
  dataWorker->addDataSet(grantoveSchemaDataSet);
  dataWorker->addDataSet(dotaceTitulDataSet);
  dataWorker->addDataSet(poskytovatelDotaceDataSet);
  Logger::log(Debug, "Add datasets to data worker", true);

  auto dataWriter = new CsvWriter(csvPath + "all.csv");
  std::string query = QUERY;
  dataWorker->writeResult(*dataWriter,
                          query);

  Logger::log(Debug, "Written query: " + query, true);

  delete dataWriter;
  dataWriter = new CsvWriter(csvPath + "all_agr.csv");
  query = QUERY_AGR;
  dataWorker->writeResult(*dataWriter,
                          query);
  Logger::log(Debug, "Written query: " + query, true);
  delete dataWriter;

  dataWriter = new CsvWriter(csvPath + "2017.csv");
  query = QUERY_2017;
  dataWorker->writeResult(*dataWriter,
                          query);
  Logger::log(Debug, "Written query: " + query, true);
  delete dataWriter;

  dataWriter = new CsvWriter(csvPath + "2017_agr.csv");
  query = QUERY_AGR_2017;
  dataWorker->writeResult(*dataWriter,
                          query);
  Logger::log(Debug, "Written query: " + query, true);
  delete dataWriter;

  delete dataWorker;
  delete subjektyProvider;

  delete operacniProgramDataSet;
  delete grantoveSchemaDataSet;
  delete dotaceTitulDataSet;
  delete poskytovatelDotaceDataSet;
  Logger::log(Debug, "Done", true);
  return 0;
}
