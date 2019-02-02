

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
    "SELECT main.idDotace, main.projektKod, main.idPrijemce, main.projektNazev, main.iriOperacniProgram, "
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

void countIntersection() {
  auto prijemceProvider =
      new DataProviders::CsvReader(csvPath + "2017_no_filter.csv", ",");
  auto subjektyProvider =
      new DataProviders::CsvReader(csvPath + subjektyCSVName, ";");

  auto prijemceDataSet = new DataSets::MemoryDataSet("prijemce");
  prijemceDataSet->setDataProvider(prijemceProvider);
  prijemceDataSet->setFieldTypes({ValueType::String, ValueType::String,
                                  ValueType::String,
                                  ValueType::String, ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String, ValueType::String,
                                  ValueType::String,
                                  ValueType::String, ValueType::String,
                                  ValueType::Integer,
                                  ValueType::String});
  prijemceDataSet->open();
  Logger::getInstance().log(LogLevel::Debug, "Prijemce loaded", true);

  auto subjektyDataSet = new DataSets::MemoryDataSet("subjekty");
  subjektyDataSet->setDataProvider(subjektyProvider);
  subjektyDataSet->setFieldTypes({ValueType::Integer, ValueType::Integer,
                                  ValueType::Integer,
                                  ValueType::String, ValueType::String,
                                  ValueType::String,
                                  ValueType::String,
                                  ValueType::String, ValueType::String});
  subjektyDataSet->open();
  Logger::getInstance().log(LogLevel::Debug, "Subjekty loaded", true);

  DataSets::SortOptions options;
  options.addOption(prijemceDataSet->fieldByName("ico")->getIndex(),
                    SortOrder::Ascending);
  prijemceDataSet->sort(options);

  DataSets::SortOptions options2;
  options2.addOption(subjektyDataSet->fieldByName("IČO_num")->getIndex(),
                     SortOrder::Ascending);
  subjektyDataSet->sort(options2);

  Logger::getInstance().log(LogLevel::Debug, "Sorted", true);

  auto fieldPrijemce =
      dynamic_cast<DataSets::IntegerField *>(prijemceDataSet->fieldByName("ico"));
  auto fieldSubjekty =
      dynamic_cast<DataSets::IntegerField *>(subjektyDataSet->fieldByName(
          "IČO_num"));
  int cnt = 0;
  int lastFound = 0;
  while (!prijemceDataSet->eof()) {

    while (!subjektyDataSet->eof()) {

      switch (Utilities::compareInt(fieldPrijemce->getAsInteger(),
                                    fieldSubjekty->getAsInteger())) {
        case 0:
          if (lastFound != fieldPrijemce->getAsInteger()) {
            cnt++;
          }
          goto man;
        case -1:goto man;
        case 1:goto uganda;
      }
      uganda:
      subjektyDataSet->next();
    }
    man:
    lastFound = fieldPrijemce->getAsInteger();

    if (subjektyDataSet->eof()) {
      break;
    }

    prijemceDataSet->next();
  }
  Logger::getInstance().log(LogLevel::Info,
                            "Total intersection count: " + std::to_string(cnt),
                            true);

}

void checkDupl() {
  auto prov = new DataProviders::CsvReader(
      csvPath + "dotaceTitul_rozpoctovaSkladbaPolozka.csv", ",");
  auto dataset = new DataSets::MemoryDataSet("tmp");
  dataset->setDataProvider(prov);
  dataset->setFieldTypes({ValueType::String, ValueType::String});
  dataset->open();

  auto field = dataset->fieldByIndex(0);

  DataSets::SortOptions options;
  options.addOption(0, SortOrder::Ascending);
  dataset->sort(options);

  std::string last = "";
  int cnt = 0, cnt2 = 0, total = 0;
  bool logged = false;
  while (!dataset->eof()) {
    total++;
    if (Utilities::compareString(field->getAsString(), last) == 0) {
      if (!logged) {
        //Logger::getInstance().log(LogLevel::Warning, "Duplicate: " + last);
        cnt++;
        logged = true;
      }
      cnt2++;
    } else {
      logged = false;
    }

    last = field->getAsString();
    dataset->next();
  }

  Logger::getInstance().log(LogLevel::Warning,
                            "Record count: " + std::to_string(total));
  Logger::getInstance().log(LogLevel::Warning,
                            "Duplicate count: " + std::to_string(cnt2));
  Logger::getInstance().log(LogLevel::Warning,
                            "Unique duplicate count: " + std::to_string(cnt));
  Logger::getInstance().log(LogLevel::Warning,
                            "Avg on duplicates: "
                                + std::to_string(cnt2 / (double) cnt));

  delete prov;
  delete dataset;
}

void dominikKontrola() {
  std::ifstream kontrola;

  std::string kontrolaLine;
  std::string buffer;

  std::unordered_set<std::string> kontrolaSet;

  int pos, counter = 0;

  kontrola.open("/Users/petr/Desktop/kontrola.csv", std::ios::out);

  while (getline(kontrola, kontrolaLine)) {
    pos = kontrolaLine.find(",");
    buffer = kontrolaLine.substr(0, pos);
    if (kontrolaSet.count(buffer)) {
      counter++;
      continue;
    }
    kontrolaSet.insert(buffer);
  }
  //printf("%d \n", counter);
}

void writerTest() {
  auto prov = new DataProviders::CsvReader(
      csvPath + "dotaceTitul_rozpoctovaSkladbaPolozka.csv", ",");

  auto writer = new DataWriters::CsvWriter("/Users/petr/Desktop/output.csv");
  writer->writeHeader({"jemen"});

  std::array<int, 1> colIndices = {0};
  std::transform(prov->begin(),
                 prov->end(),
                 writer->begin(),
                 [&colIndices](std::vector<std::string> row) -> std::vector<std::string> {
                   std::vector<std::string> subSet;
                   subSet.reserve(colIndices.size());
                   for (auto index : colIndices) {
                     subSet.emplace_back(row[index]);
                   }
                   return subSet;
                 });
  delete writer;
  delete prov;
}

void measure(int index) {

  std::vector<std::vector<std::string>> data;

  std::vector<int> cols{0, 3};

  int dataSize = 50000;

  std::vector<std::string> tmp{
      "10", "10", "10", "10", "10", "10", "10", "10", "10", "10"};
  for (auto i = 0; i < dataSize; ++i) {
    data.emplace_back(tmp);
  }

  DataProviders::ArrayDataProvider prov(data);
  int reps = 1000;
  auto startTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch());
  std::vector<std::vector<std::string>> man;
  man.reserve(dataSize);

  tmp.reserve(cols.size());
  for (auto i = 0; i < reps; ++i) {
    while (prov.next()) {
      for (const auto &col : cols) {
        tmp.push_back(prov.getRow()[col]);
      }
      man.push_back(tmp);
      tmp.clear();
    }
    std::cout << man[index][0];
    man.clear();
    prov.first();
  }
  auto endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch());

  std::cout << std::endl << "count1:" << (endTimeMs - startTimeMs).count()
            << std::endl;

  prov.first();
  startTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch());

  std::vector<std::string> subSet;
  subSet.reserve(cols.size());
  auto pickColsFunc = [&cols, &subSet](const std::vector<std::string> &row) {
    subSet.clear();
    for (auto index : cols) {
      subSet.push_back(row[index]);
    }
    return subSet;
  };
  for (auto i = 0; i < reps; ++i) {
    std::transform(prov.begin(),
                   prov.end(),
                   std::back_inserter(man),
                   pickColsFunc);
    std::cout << man[index][0];
    subSet.clear();
    prov.first();
  }
  endTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now().time_since_epoch());
  std::cout << std::endl << "count2:" << (endTimeMs - startTimeMs).count()
            << std::endl;
}

int main(int argc, char **argv) {
  measure(argc);
  return 0;
  writerTest();
  for (auto i = 0; i < 1; ++i) {
    Logger::getInstance().startTime();
    //iterExample();
    //checkDupl();
    //dominikKontrola();
    //countIntersection();
    Logger::getInstance().endTime();
    Logger::getInstance().printElapsedTime();
  }
  return 0;
  Logger::getInstance().startTime();
  // open input files
  auto dotaceProvider =
      new DataProviders::CsvReader(csvPath + dotaceCSVName, ",");
  auto rozhodnutiProvider =
      new DataProviders::CsvReader(csvPath + rozhodnutiCSVName, ",");
  auto obdobiProvider =
      new DataProviders::CsvReader(csvPath + obdobiCSVName, ",");
  auto prijemceProvider =
      new DataProviders::CsvReader(csvPath + prijemceCSVName, ",");
  auto subjektyProvider =
      new DataProviders::CsvReader(csvPath + subjektyCSVName, ";");

  Logger::getInstance().log(LogLevel::Debug, "Providers prepared", true);
  auto dotaceDataSet = new DataSets::MemoryDataSet("dotace");
  dotaceDataSet->setDataProvider(dotaceProvider);
  dotaceDataSet->setFieldTypes({ValueType::String, ValueType::String,
                                ValueType::String,
                                ValueType::String, ValueType::String,
                                ValueType::String});
  dotaceDataSet->open();
  Logger::getInstance().log(LogLevel::Debug, "Dotace loaded", true);
  auto rozhodnutiDataSet = new DataSets::MemoryDataSet("rozhodnuti");
  rozhodnutiDataSet->setDataProvider(rozhodnutiProvider);
  rozhodnutiDataSet->setFieldTypes({ValueType::String, ValueType::String,
                                    ValueType::String});
  rozhodnutiDataSet->open();
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
  obdobiDataSet->setDataProvider(obdobiProvider);
  obdobiDataSet->setFieldTypes({ValueType::String, ValueType::String,
                                ValueType::Currency,
                                ValueType::Currency, ValueType::String,
                                ValueType::String});
  obdobiDataSet->open();
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
  prijemceDataSet->setDataProvider(prijemceProvider);
  prijemceDataSet->setFieldTypes({ValueType::String, ValueType::Integer,
                                  ValueType::String});
  prijemceDataSet->open();
  Logger::getInstance().log(LogLevel::Debug, "Prijemce loaded", true);
  auto subjektyDataSet = new DataSets::MemoryDataSet("subjekty");
  subjektyDataSet->setDataProvider(subjektyProvider);
  subjektyDataSet->setFieldTypes({ValueType::Integer, ValueType::Integer,
                                  ValueType::Integer,
                                  ValueType::String, ValueType::String,
                                  ValueType::String,
                                  ValueType::String});
  subjektyDataSet->open();
  Logger::getInstance().log(LogLevel::Debug, "Subjekty loaded", true);
  merger.addDataSet(subjektyDataSet);
  merger.addDataSet(prijemceDataSet);
  auto prijemce_subjektyDataSet =
      merger.mergeDataSets("prijemce", "subjekty", "ico", "IČO_num");
  Logger::getInstance().log(LogLevel::Debug, "Merged prijemce, subjekty", true);
  delete subjektyDataSet;
  delete prijemceDataSet;

  auto operacniProgramProvider =
      new DataProviders::CsvReader(csvPath + operacniProgramCSVName, ",");
  auto operacniProgramDataSet = new DataSets::MemoryDataSet("operacniProgram");
  operacniProgramDataSet->setDataProvider(operacniProgramProvider);
  operacniProgramDataSet->setFieldTypes({ValueType::String, ValueType::String});
  operacniProgramDataSet->open();
  Logger::getInstance().log(LogLevel::Debug, "operacniProgram loaded", true);
  auto grantoveSchemaProvider =
      new DataProviders::CsvReader(csvPath + grantoveSchemaCSVName, ",");
  auto grantoveSchemaDataSet = new DataSets::MemoryDataSet("grantoveSchema");
  grantoveSchemaDataSet->setDataProvider(grantoveSchemaProvider);
  grantoveSchemaDataSet->setFieldTypes({ValueType::String, ValueType::String});
  grantoveSchemaDataSet->open();
  Logger::getInstance().log(LogLevel::Debug, "grantoveSchema loaded", true);
  auto dotaceTitulProvider =
      new DataProviders::CsvReader(csvPath + dotacniTitulCSVName, ",");
  auto dotaceTitulDataSet = new DataSets::MemoryDataSet("dotaceTitul");
  dotaceTitulDataSet->setDataProvider(dotaceTitulProvider);
  dotaceTitulDataSet->setFieldTypes({ValueType::String, ValueType::String});
  dotaceTitulDataSet->open();
  Logger::getInstance().log(LogLevel::Debug, "dotaceTitul loaded", true);
  auto poskytovatelDotaceProvider =
      new DataProviders::CsvReader(csvPath + poskytovatelDotaceCSVName, ",");
  auto poskytovatelDotaceDataSet =
      new DataSets::MemoryDataSet("poskytovatelDotace");
  poskytovatelDotaceDataSet->setDataProvider(poskytovatelDotaceProvider);
  poskytovatelDotaceDataSet->setFieldTypes({ValueType::String,
                                            ValueType::String});
  poskytovatelDotaceDataSet->open();
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
  delete subjektyProvider;
  delete operacniProgramDataSet;
  delete grantoveSchemaDataSet;
  delete dotaceTitulDataSet;
  delete poskytovatelDotaceDataSet;
  Logger::getInstance().log(LogLevel::Debug, "Done", true);
  Logger::getInstance().endTime();
  Logger::getInstance().printElapsedTime();
  return 0;
}