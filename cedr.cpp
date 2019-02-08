

#include <iostream>
#include <iomanip>
#include <MemoryDataSet.h>
#include <DataSetMerger.h>
#include "DataProviders/Headers/CsvReader.h"
#include <Logger.h>

#include <set>
#include <unordered_set>
#include <XlsxWriter.h>
#include <BaseFileDownloader.h>
#include <ArrayDataProvider.h>

const std::string csvPath = "/Users/petr/Desktop/csvs/";
const std::string outPath = csvPath + "out/";
const std::string dotaceCSVName = "dotace.csv";
const std::string rozhodnutiCSVName = "rozhodnuti.csv";
const std::string obdobiCSVName = "rozpoctoveObdobi.csv";
const std::string prijemceCSVName = "prijemcePomoci.csv";
const std::string outputCSVName = "cedr_output.csv";
const std::string outputCSVAgrName = "cedr_output_agr.csv";
const std::string subjektyCSVName = "NNO_subjekty6ver2.4.csv";

const std::string operacniProgramCSVName = "operacniProgram.csv";
const std::string grantoveSchemaCSVName = "grantoveSchema.csv";
const std::string dotacniTitulCSVName = "dotaceTitul.csv";
const std::string poskytovatelDotaceCSVName = "poskytovatelDotace.csv";

const std::string
    QUERY =
    "SELECT main.idDotace, main.projektIdnetifikator, main.idPrijemce, main.projektNazev, main.iriOperacniProgram, "
    "main.iriGrantoveSchema, main.idRozhodnuti, main.castkaCerpana, main.castkaUvolnena, "
    "main.iriDotacniTitul, main.iriPoskytovatelDotace, main.rozpoctoveObdobi, "
    "prijemce_subjekty.idPrijemce, prijemce_subjekty.ico, prijemce_subjekty.obchodniJmeno, "
    "prijemce_subjekty.Id_GIS, prijemce_subjekty.Y, prijemce_subjekty.X, "
    "operacniProgram.operacniProgramNazev, "
    "grantoveSchema.grantoveSchemaNazev, "
    "dotaceTitul.dotaceTitulNazev "
    "poskytovatelDotace.dotacePoskytovatelNazev "
    "FROM main "
    "JOIN prijemce_subjekty ON main.idPrijemce = prijemce_subjekty.idPrijemce "
    "LEFT JOIN operacniProgram ON main.iriOperacniProgram = operacniProgram.idOperacniProgram "
    "LEFT JOIN grantoveSchema ON main.iriGrantoveSchema = grantoveSchema.idGrantoveSchema "
    "LEFT JOIN dotaceTitul ON main.iriDotacniTitul = dotaceTitul.idDotaceTitul "
    "LEFT JOIN poskytovatelDotace ON main.iriPoskytovatelDotace = poskytovatelDotace.id";

const std::string QUERY_2017 = QUERY + " WHERE main.rozpoctoveObdobi = 2017";

const std::string QUERY_AGR =
    "SELECT main.idPrijemce, SUM(main.castkaCerpana), SUM(main.castkaUvolnena), "
    "main.rozpoctoveObdobi, "
    "prijemce_subjekty.idPrijemce, prijemce_subjekty.ico, prijemce_subjekty.obchodniJmeno, "
    "prijemce_subjekty.Id_GIS, prijemce_subjekty.Y, prijemce_subjekty.X "
    //"operacniProgram.operacniProgramNazev, "
    //"grantoveSchema.grantoveSchemaNazev, "
    //"dotaceTitul.dotaceTitulNazev, "
    //"poskytovatelDotace.dotacePoskytovatelNazev "
    "FROM main "
    "JOIN prijemce_subjekty ON main.idPrijemce = prijemce_subjekty.idPrijemce";
//"LEFT JOIN operacniProgram ON main.iriOperacniProgram = operacniProgram.idOperacniProgram "
//"LEFT JOIN grantoveSchema ON main.iriGrantoveSchema = grantoveSchema.idGrantoveSchema "
//"LEFT JOIN dotaceTitul ON main.iriDotacniTitul = dotaceTitul.idDotaceTitul "
//"LEFT JOIN poskytovatelDotace ON main.iriPoskytovatelDotace = poskytovatelDotace.id";

const std::string
    QUERY_AGR_2017 = QUERY_AGR + " WHERE main.rozpoctoveObdobi = 2017";

int main(int argc, char **argv) {
  Logger::getInstance().startTime();
  // open input files
  DataProviders::CsvReader dotaceProvider(csvPath + dotaceCSVName, ",");
  DataProviders::CsvReader rozhodnutiProvider(csvPath + rozhodnutiCSVName, ",");
  DataProviders::CsvReader obdobiProvider(csvPath + obdobiCSVName, ",");
  DataProviders::CsvReader prijemceProvider(csvPath + prijemceCSVName, ",");
  DataProviders::CsvReader subjektyProvider(csvPath + subjektyCSVName, ";");

  Logger::getInstance().log(LogLevel::Debug, "Providers prepared", true);
  auto dotaceDataSet = new DataSets::MemoryDataSet("dotace");
  dotaceDataSet->open(dotaceProvider, {ValueType::String, ValueType::String,
                                       ValueType::String,
                                       ValueType::String, ValueType::String,
                                       ValueType::String});
  Logger::getInstance().log(LogLevel::Debug, "Dotace loaded", true);

  auto rozhodnutiDataSet = new DataSets::MemoryDataSet("rozhodnuti");
  rozhodnutiDataSet->open(rozhodnutiProvider, {ValueType::String,
                                               ValueType::String,
                                               ValueType::String});
  Logger::getInstance().log(LogLevel::Debug, "Rozhodnuti loaded", true);

  DataWorkers::DataSetMerger merger;
  merger.addDataSet(dotaceDataSet);
  merger.addDataSet(rozhodnutiDataSet);
  auto dotace_rozhodnutiDataSet =
      merger.mergeDataSets("dotace", "rozhodnuti", "idDotace", "idDotace");
  Logger::getInstance().log(LogLevel::Debug, "Merged dotace, rozhodnuti", true);

  merger.removeDataSet("dotace");
  merger.removeDataSet("rozhodnuti");
  delete dotaceDataSet;
  delete rozhodnutiDataSet;

  auto obdobiDataSet = new DataSets::MemoryDataSet("obdobi");
  obdobiDataSet->open(obdobiProvider, {ValueType::String,
                                       ValueType::String,
                                       ValueType::Currency,
                                       ValueType::Currency,
                                       ValueType::String,
                                       ValueType::String});
  Logger::getInstance().log(LogLevel::Debug, "Obdobi loaded", true);

  merger.addDataSet(dotace_rozhodnutiDataSet);
  merger.addDataSet(obdobiDataSet);
  auto dotace_rozhodnuti_obdobiDataSet =
      merger.mergeDataSets("dotace_rozhodnuti",
                           "obdobi",
                           "idRozhodnuti",
                           "idRozhodnuti");
  Logger::getInstance().log(LogLevel::Debug,
                            "Merged dotace_rozhodnuti, obdobi",
                            true);
  merger.removeDataSet("dotace_rozhodnuti");
  merger.removeDataSet("obdobi");
  delete obdobiDataSet;
  delete dotace_rozhodnutiDataSet;

  auto dataWorker =
      new DataWorkers::MemoryDataWorker(dotace_rozhodnuti_obdobiDataSet);

  auto prijemceDataSet = new DataSets::MemoryDataSet("prijemce");
  prijemceDataSet->open(prijemceProvider, {ValueType::String,
                                           ValueType::Integer,
                                           ValueType::String});
  Logger::getInstance().log(LogLevel::Debug, "Prijemce loaded", true);

  auto subjektyDataSet = new DataSets::MemoryDataSet("subjekty");
  subjektyDataSet->open(subjektyProvider, {ValueType::Integer,
                                           ValueType::Integer,
                                           ValueType::Integer,
                                           ValueType::String,
                                           ValueType::String,
                                           ValueType::String,
                                           ValueType::String});
  Logger::getInstance().log(LogLevel::Debug, "Subjekty loaded", true);

  merger.addDataSet(subjektyDataSet);
  merger.addDataSet(prijemceDataSet);
  auto prijemce_subjektyDataSet =
      merger.mergeDataSets("prijemce", "subjekty", "ico", "IČO_num");
  Logger::getInstance().log(LogLevel::Debug, "Merged prijemce, subjekty", true);
  delete subjektyDataSet;
  delete prijemceDataSet;

  DataProviders::CsvReader
      operacniProgramProvider(csvPath + operacniProgramCSVName, ",");
  auto operacniProgramDataSet = new DataSets::MemoryDataSet("operacniProgram");
  operacniProgramDataSet->open(operacniProgramProvider, {ValueType::String,
                                                         ValueType::String});
  Logger::getInstance().log(LogLevel::Debug, "operacniProgram loaded", true);

  DataProviders::CsvReader
      grantoveSchemaProvider(csvPath + grantoveSchemaCSVName, ",");
  auto grantoveSchemaDataSet = new DataSets::MemoryDataSet("grantoveSchema");
  grantoveSchemaDataSet->open(grantoveSchemaProvider, {ValueType::String,
                                                       ValueType::String});
  Logger::getInstance().log(LogLevel::Debug, "grantoveSchema loaded", true);

  DataProviders::CsvReader
      dotaceTitulProvider(csvPath + dotacniTitulCSVName, ",");
  auto dotaceTitulDataSet = new DataSets::MemoryDataSet("dotaceTitul");
  dotaceTitulDataSet->open(dotaceTitulProvider, {ValueType::String,
                                                 ValueType::String});
  Logger::getInstance().log(LogLevel::Debug, "dotaceTitul loaded", true);

  DataProviders::CsvReader
      poskytovatelDotaceProvider(csvPath + poskytovatelDotaceCSVName, ",");
  auto poskytovatelDotaceDataSet =
      new DataSets::MemoryDataSet("poskytovatelDotace");
  poskytovatelDotaceDataSet->open(poskytovatelDotaceProvider,
                                  {ValueType::String,
                                   ValueType::String});
  Logger::getInstance().log(LogLevel::Debug, "poskytovatelDotace loaded", true);

  dataWorker->addDataSet(prijemce_subjektyDataSet);
  dataWorker->addDataSet(operacniProgramDataSet);
  dataWorker->addDataSet(grantoveSchemaDataSet);
  dataWorker->addDataSet(dotaceTitulDataSet);
  dataWorker->addDataSet(poskytovatelDotaceDataSet);
  Logger::getInstance().log(LogLevel::Debug,
                            "Add datasets to data worker",
                            true);
  auto dataWriter = new DataWriters::CsvWriter(outPath + "all.csv", ",");
  std::string query = QUERY;
  dataWorker->writeResult(*dataWriter,
                          query);

  Logger::getInstance().log(LogLevel::Debug, "Written query: " + query, true);
  delete dataWriter;
  dataWriter = new DataWriters::CsvWriter(outPath + "all_agr.csv", ",");
  query = QUERY_AGR;
  dataWorker->writeResult(*dataWriter,
                          query);
  Logger::getInstance().log(LogLevel::Debug, "Written query: " + query, true);
  delete dataWriter;

  dataWriter = new DataWriters::CsvWriter(outPath + "2017.csv", ",");
  query = QUERY_2017;
  dataWorker->writeResult(*dataWriter,
                          query);
  Logger::getInstance().log(LogLevel::Debug, "Written query: " + query, true);
  delete dataWriter;

  dataWriter = new DataWriters::CsvWriter(outPath + "2017_agr.csv", ",");
  query = QUERY_AGR_2017;
  dataWorker->writeResult(*dataWriter,
                          query);
  Logger::getInstance().log(LogLevel::Debug, "Written query: " + query, true);
  delete dataWriter;

  delete dataWorker;
  delete operacniProgramDataSet;
  delete grantoveSchemaDataSet;
  delete dotaceTitulDataSet;
  delete poskytovatelDotaceDataSet;
  delete prijemce_subjektyDataSet;
  Logger::getInstance().log(LogLevel::Debug, "Done", true);
  Logger::getInstance().endTime();
  Logger::getInstance().printElapsedTime();
  return 0;
}
