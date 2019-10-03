//
// Created by Petr Flajsingr on 02/10/2019.
//
#include <CsvReader.h>
#include <CsvStreamReader.h>
#include <MemoryDataBase.h>
#include <MemoryDataSet.h>
#include <XlsxIOReader.h>
#include <XlsxIOWriter.h>
#include <string>
#include <Logger.h>
#include <CsvWriter.h>

using namespace std::string_literals;

const std::string csvPath = "/home/petr/Desktop/cedr/";
const std::string outPath = csvPath + "out/";
const std::string dotaceCSVName = "Dotace.csv";
const std::string rozhodnutiCSVName = "Rozhodnuti.csv";
const std::string obdobiCSVName = "RozpoctoveObdobi.csv";
const std::string prijemceCSVName = "PrijemcePomoci.csv";
const std::string outputCSVName = "cedr_output.csv";
const std::string outputCSVAgrName = "cedr_output_agr.csv";
const std::string subjektyCSVName = "NNO.xlsx";

const std::string operacniProgramCSVName = "ciselnikCedrOperacniProgramv01.csv";
const std::string grantoveSchemaCSVName = "ciselnikCedrGrantoveSchemav01.csv";
const std::string dotacniTitulCSVName = "ciselnikDotaceTitulv01.csv";
const std::string poskytovatelDotaceCSVName = "ciselnikDotacePoskytovatelv01.csv";

const std::string QUERY =
    "SELECT dotace.idDotace, dotace.projektIdnetifikator, dotace.idPrijemce, "
    "dotace.projektNazev, dotace.iriOperacniProgram, "
    "dotace.iriGrantoveSchema, rozhodnuti.idRozhodnuti, obdobi.castkaCerpana, "
    "obdobi.castkaUvolnena, obdobi.iriDotacniTitul, rozhodnuti.iriPoskytovatelDotace, "
    "obdobi.rozpoctoveObdobi, "
    "prijemce.idPrijemce, prijemce.obchodniJmeno, "
    "subjekty.*, "
    "operacniProgram.operacniProgramNazev, "
    "grantoveSchema.grantoveSchemaNazev, "
    "dotaceTitul.dotaceTitulNazev, "
    "poskytovatelDotace.dotacePoskytovatelNazev "
    "FROM dotace "
    "JOIN prijemce ON dotace.idPrijemce = prijemce.idPrijemce "
    "JOIN subjekty ON prijemce.ico = subjekty.ICOnum "
    "JOIN rozhodnuti ON dotace.idDotace = rozhodnuti.idDotace "
    "JOIN obdobi ON rozhodnuti.idRozhodnuti = obdobi.idRozhodnuti "
    "LEFT JOIN operacniProgram ON dotace.iriOperacniProgram = operacniProgram.idOperacniProgram "
    "LEFT JOIN grantoveSchema ON dotace.iriGrantoveSchema = grantoveSchema.idGrantoveSchema "
    "LEFT JOIN dotaceTitul ON obdobi.iriDotacniTitul = dotaceTitul.idDotaceTitul "
    "LEFT JOIN poskytovatelDotace ON rozhodnuti.iriPoskytovatelDotace = poskytovatelDotace.id";

const std::string QUERY_2018 = QUERY + " WHERE obdobi.rozpoctoveObdobi = 2017;";

const std::string QUERY_AGR =
    "SELECT dotace.idPrijemce, SUM(dotace.castkaCerpana), "
    "SUM(dotace.castkaUvolnena), "
    "dotace.rozpoctoveObdobi, "
    "prijemce_subjekty.idPrijemce, prijemce_subjekty.ico, "
    "prijemce_subjekty.obchodniJmeno, "
    "prijemce_subjekty.Id_GIS, prijemce_subjekty.Y, prijemce_subjekty.X "
    "FROM dotace "
    "JOIN prijemce_subjekty ON dotace.idPrijemce = "
    "prijemce_subjekty.idPrijemce";

const std::string QUERY_AGR_2018 =
    QUERY_AGR + " WHERE dotace.rozpoctoveObdobi = 2018";

std::shared_ptr<DataSets::MemoryDataSet> make_dataset(const std::string &name) {
  return std::make_shared<DataSets::MemoryDataSet>(name);
}

auto logger = Logger<true>::GetInstance();

int main() {
  DataBase::MemoryDataBase dataBase("db");

  DataProviders::CsvStreamReader dotaceProvider(csvPath + dotaceCSVName, ',');
  DataProviders::CsvStreamReader rozhodnutiProvider(csvPath + rozhodnutiCSVName, ',');
  DataProviders::CsvStreamReader obdobiProvider(csvPath + obdobiCSVName, ',');
  DataProviders::CsvStreamReader prijemceProvider(csvPath + prijemceCSVName, ',');
  DataProviders::XlsxIOReader subjektyProvider(csvPath + subjektyCSVName);

  auto dotaceDataSet = make_dataset("dotace");
  dotaceDataSet->open(dotaceProvider, {ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String});
  dataBase.addTable(dotaceDataSet);
    logger.log<LogLevel::Status>("Loaded: "s + dotaceDataSet->getName());
    dotaceDataSet->resetEnd();
    logger.log<LogLevel::Debug>("Count:", dotaceDataSet->getCurrentRecord());


  auto rozhodnutiDataSet = make_dataset("rozhodnuti");
  rozhodnutiDataSet->open(
      rozhodnutiProvider,
      {ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String});
  dataBase.addTable(rozhodnutiDataSet);
    logger.log<LogLevel::Status>("Loaded: "s + rozhodnutiDataSet->getName());
    rozhodnutiDataSet->resetEnd();
    logger.log<LogLevel::Debug>("Count:", rozhodnutiDataSet->getCurrentRecord());

  auto obdobiDataSet = make_dataset("obdobi");
  obdobiDataSet->open(obdobiProvider, {ValueType::String,
                                       ValueType::String,
                                       ValueType::Currency,
                                       ValueType::Currency,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String,
                                       ValueType::String});
  dataBase.addTable(obdobiDataSet);
    logger.log<LogLevel::Status>("Loaded: "s + obdobiDataSet->getName());
    obdobiDataSet->resetEnd();
    logger.log<LogLevel::Debug>("Count:", obdobiDataSet->getCurrentRecord());

  auto prijemceDataSet = make_dataset("prijemce");
  prijemceDataSet->open(
      prijemceProvider,
      {ValueType::String,
       ValueType::Integer,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,
       ValueType::String,});
  dataBase.addTable(prijemceDataSet);
    logger.log<LogLevel::Status>("Loaded: "s + prijemceDataSet->getName());
    prijemceDataSet->resetEnd();
    logger.log<LogLevel::Debug>("Count:", prijemceDataSet->getCurrentRecord());

  auto subjektyDataSet = make_dataset("subjekty");
  subjektyDataSet->open(subjektyProvider,
                        {ValueType::Integer,
                         ValueType::Integer,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,
                         ValueType::String,});
  dataBase.addTable(subjektyDataSet);
    logger.log<LogLevel::Status>("Loaded: "s + subjektyDataSet->getName());
    subjektyDataSet->resetEnd();
    logger.log<LogLevel::Debug>("Count:", subjektyDataSet->getCurrentRecord());

  DataProviders::CsvReader operacniProgramProvider(
      csvPath + operacniProgramCSVName, ",");
  auto operacniProgramDataSet = make_dataset("operacniProgram");
  operacniProgramDataSet->open(operacniProgramProvider,
                               {ValueType::String,
                                ValueType::String,
                                ValueType::String,
                                ValueType::String,
                                ValueType::String,
                                ValueType::String,});
  dataBase.addTable(operacniProgramDataSet);
    logger.log<LogLevel::Status>("Loaded: "s + operacniProgramDataSet->getName());
    operacniProgramDataSet->resetEnd();
    logger.log<LogLevel::Debug>("Count:", operacniProgramDataSet->getCurrentRecord());

  DataProviders::CsvReader grantoveSchemaProvider(
      csvPath + grantoveSchemaCSVName, ",");
  auto grantoveSchemaDataSet = make_dataset("grantoveSchema");
  grantoveSchemaDataSet->open(grantoveSchemaProvider,
                              {ValueType::String,
                               ValueType::String,
                               ValueType::String,
                               ValueType::String,
                               ValueType::String,
                               ValueType::String,});
  dataBase.addTable(grantoveSchemaDataSet);
    logger.log<LogLevel::Status>("Loaded: "s + grantoveSchemaDataSet->getName());
    grantoveSchemaDataSet->resetEnd();
    logger.log<LogLevel::Debug>("Count:", grantoveSchemaDataSet->getCurrentRecord());

  DataProviders::CsvReader dotaceTitulProvider(csvPath + dotacniTitulCSVName,
                                               ",");
  auto dotaceTitulDataSet = make_dataset("dotaceTitul");
  dotaceTitulDataSet->open(dotaceTitulProvider,
                           {ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,
                            ValueType::String,});
  dataBase.addTable(dotaceTitulDataSet);
    logger.log<LogLevel::Status>("Loaded: "s + dotaceTitulDataSet->getName());
    dotaceTitulDataSet->resetEnd();
    logger.log<LogLevel::Debug>("Count:", dotaceTitulDataSet->getCurrentRecord());

  DataProviders::CsvReader poskytovatelDotaceProvider(
      csvPath + poskytovatelDotaceCSVName, ",");
  auto poskytovatelDotaceDataSet = make_dataset("poskytovatelDotace");
  poskytovatelDotaceDataSet->open(poskytovatelDotaceProvider,
                                  {ValueType::String,
                                   ValueType::String,
                                   ValueType::String,
                                   ValueType::String,
                                   ValueType::String,
                                   ValueType::String,});
  dataBase.addTable(poskytovatelDotaceDataSet);
    logger.log<LogLevel::Status>("Loaded: "s + poskytovatelDotaceDataSet->getName());
    poskytovatelDotaceDataSet->resetEnd();
    logger.log<LogLevel::Debug>("Count:", poskytovatelDotaceDataSet->getCurrentRecord());

  auto result = dataBase.execSimpleQuery(QUERY_2018, false, "cedr");
  DataWriters::CsvWriter writer(outPath + outputCSVName);

  writer.writeHeader(result->dataSet->getFieldNames());
  auto fields = result->dataSet->getFields();
  result->dataSet->resetBegin();

  std::vector<std::string> record;
  record.reserve(fields.size());
  while (result->dataSet->next()) {
    std::transform(
        fields.begin(), fields.end(), std::back_inserter(record),
        [](const DataSets::BaseField *field) { return field->getAsString(); });
    writer.writeRecord(record);
    record.clear();
  }
}