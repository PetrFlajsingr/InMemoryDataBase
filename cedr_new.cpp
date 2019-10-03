//
// Created by Petr Flajsingr on 02/10/2019.
//
#include <CsvReader.h>
#include <MemoryDataBase.h>
#include <MemoryDataSet.h>
#include <XlsxIOReader.h>
#include <XlsxIOWriter.h>
#include <string>

const std::string csvPath = "/Users/petr/Desktop/cedr/";
const std::string outPath = csvPath + "out/";
const std::string dotaceCSVName = "dotace.csv";
const std::string rozhodnutiCSVName = "rozhodnuti.csv";
const std::string obdobiCSVName = "rozpoctoveObdobi.csv";
const std::string prijemceCSVName = "prijemcePomoci.csv";
const std::string outputCSVName = "cedr_output.csv";
const std::string outputCSVAgrName = "cedr_output_agr.csv";
const std::string subjektyCSVName = "NNO.xlsx";

const std::string operacniProgramCSVName = "operacniProgram.csv";
const std::string grantoveSchemaCSVName = "grantoveSchema.csv";
const std::string dotacniTitulCSVName = "dotaceTitul.csv";
const std::string poskytovatelDotaceCSVName = "poskytovatelDotace.csv";

const std::string QUERY =
    "SELECT dotace.idDotace, dotace.projektIdnetifikator, dotace.idPrijemce, "
    "dotace.projektNazev, dotace.iriOperacniProgram, "
    "dotace.iriGrantoveSchema, dotace.idRozhodnuti, dotace.castkaCerpana, "
    "dotace.castkaUvolnena, "
    "dotace.iriDotacniTitul, dotace.iriPoskytovatelDotace, "
    "dotace.rozpoctoveObdobi, "
    "prijemce.idPrijemce, "
    "prijemce.obchodniJmeno, "
    "subjekty.*, "
    "operacniProgram.operacniProgramNazev, "
    "grantoveSchema.grantoveSchemaNazev, "
    "dotaceTitul.dotaceTitulNazev "
    "poskytovatelDotace.dotacePoskytovatelNazev "
    "FROM dotace "
    "JOIN prijemce ON dotace.idPrijemce = prijemce.idPrijemce "
    "JOIN subjekty ON prijemce.ico = subjekty.ICOnum "
    "LEFT JOIN operacniProgram ON dotace.iriOperacniProgram = "
    "operacniProgram.idOperacniProgram "
    "LEFT JOIN grantoveSchema ON dotace.iriGrantoveSchema = "
    "grantoveSchema.idGrantoveSchema "
    "LEFT JOIN dotaceTitul ON dotace.iriDotacniTitul = "
    "dotaceTitul.idDotaceTitul "
    "LEFT JOIN poskytovatelDotace ON dotace.iriPoskytovatelDotace = "
    "poskytovatelDotace.id";

const std::string QUERY_2018 = QUERY + " WHERE dotace.rozpoctoveObdobi = 2018";

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

int main() {
  DataBase::MemoryDataBase dataBase("db");

  DataProviders::CsvReader dotaceProvider(csvPath + dotaceCSVName, ",");
  DataProviders::CsvReader rozhodnutiProvider(csvPath + rozhodnutiCSVName, ",");
  DataProviders::CsvReader obdobiProvider(csvPath + obdobiCSVName, ",");
  DataProviders::CsvReader prijemceProvider(csvPath + prijemceCSVName, ",");
  DataProviders::XlsxIOReader subjektyProvider(csvPath + subjektyCSVName);

  auto dotaceDataSet = make_dataset("dotace");
  dotaceDataSet->open(dotaceProvider, {ValueType::String, ValueType::String,
                                       ValueType::String, ValueType::String,
                                       ValueType::String, ValueType::String});
  dataBase.addTable(dotaceDataSet);

  auto rozhodnutiDataSet = make_dataset("rozhodnuti");
  rozhodnutiDataSet->open(
      rozhodnutiProvider,
      {ValueType::String, ValueType::String, ValueType::String});
  dataBase.addTable(rozhodnutiDataSet);

  auto obdobiDataSet = make_dataset("obdobi");
  obdobiDataSet->open(obdobiProvider, {ValueType::String, ValueType::String,
                                       ValueType::Currency, ValueType::Currency,
                                       ValueType::String, ValueType::String});
  dataBase.addTable(obdobiDataSet);

  auto prijemceDataSet = make_dataset("prijemce");
  prijemceDataSet->open(
      prijemceProvider,
      {ValueType::String, ValueType::Integer, ValueType::String});
  dataBase.addTable(prijemceDataSet);

  auto subjektyDataSet = make_dataset("subjekty");
  subjektyDataSet->open(subjektyProvider,
                        {ValueType::Integer, ValueType::Integer,
                         ValueType::Integer, ValueType::String,
                         ValueType::String, ValueType::String,
                         ValueType::String});
  dataBase.addTable(subjektyDataSet);

  DataProviders::CsvReader operacniProgramProvider(
      csvPath + operacniProgramCSVName, ",");
  auto operacniProgramDataSet = make_dataset("operacniProgram");
  operacniProgramDataSet->open(operacniProgramProvider,
                               {ValueType::String, ValueType::String});
  dataBase.addTable(operacniProgramDataSet);

  DataProviders::CsvReader grantoveSchemaProvider(
      csvPath + grantoveSchemaCSVName, ",");
  auto grantoveSchemaDataSet = make_dataset("grantoveSchema");
  grantoveSchemaDataSet->open(grantoveSchemaProvider,
                              {ValueType::String, ValueType::String});
  dataBase.addTable(grantoveSchemaDataSet);

  DataProviders::CsvReader dotaceTitulProvider(csvPath + dotacniTitulCSVName,
                                               ",");
  auto dotaceTitulDataSet = make_dataset("dotaceTitul");
  dotaceTitulDataSet->open(dotaceTitulProvider,
                           {ValueType::String, ValueType::String});
  dataBase.addTable(dotaceTitulDataSet);

  DataProviders::CsvReader poskytovatelDotaceProvider(
      csvPath + poskytovatelDotaceCSVName, ",");
  auto poskytovatelDotaceDataSet = make_dataset("poskytovatelDotace");
  poskytovatelDotaceDataSet->open(poskytovatelDotaceProvider,
                                  {ValueType::String, ValueType::String});
  dataBase.addTable(poskytovatelDotaceDataSet);

  auto result = dataBase.execSimpleQuery(QUERY_2018, false, "cedr");
  DataWriters::XlsxIOWriter writer(outPath + outputCSVName);

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