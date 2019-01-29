

#include <iostream>
#include <iomanip>
#include <MemoryDataSet.h>
#include <DataSetMerger.h>
#include <MemoryDataWorker.h>
#include "DataProviders/Headers/CsvReader.h"
#include <Logger.h>

#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>

#include <set>
#include <unordered_set>
#include <XlsxWriter.h>
#include <array>

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

void printMemoryUsageMacOS() {
  return;
  vm_size_t page_size;
  mach_port_t mach_port;
  mach_msg_type_number_t count;
  vm_statistics64_data_t vm_stats;

  mach_port = mach_host_self();
  count = sizeof(vm_stats) / sizeof(natural_t);
  if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
      KERN_SUCCESS == host_statistics64(mach_port, HOST_VM_INFO,
                                        (host_info64_t) &vm_stats, &count)) {
    long long free_memory = (int64_t) vm_stats.free_count * (int64_t) page_size;

    long long used_memory = ((int64_t) vm_stats.active_count +
        (int64_t) vm_stats.inactive_count +
        (int64_t) vm_stats.wire_count) * (int64_t) page_size;
    printf("free memory: %lld\nused memory: %lld\n", free_memory, used_memory);
  }
}

void countIntersection() {
  auto prijemceProvider =
      new DataProviders::CsvReader(csvPath + "2017_no_filter.csv", ",");
  auto subjektyProvider =
      new DataProviders::CsvReader(csvPath + subjektyCSVName, ";");

  auto prijemceDataSet = new DataSets::MemoryDataSet("prijemce");
  prijemceDataSet->setDataProvider(prijemceProvider);
  prijemceDataSet->setFieldTypes({StringValue, StringValue, StringValue,
                                  StringValue, StringValue, StringValue,
                                  StringValue,
                                  StringValue, StringValue, StringValue,
                                  StringValue, StringValue, IntegerValue,
                                  StringValue});
  prijemceDataSet->open();
  Logger::log(Debug, "Prijemce loaded", true);

  auto subjektyDataSet = new DataSets::MemoryDataSet("subjekty");
  subjektyDataSet->setDataProvider(subjektyProvider);
  subjektyDataSet->setFieldTypes({IntegerValue, IntegerValue, IntegerValue,
                                  StringValue, StringValue, StringValue,
                                  StringValue,
                                  StringValue, StringValue});
  subjektyDataSet->open();
  Logger::log(Debug, "Subjekty loaded", true);

  DataSets::SortOptions options;
  options.addOption(prijemceDataSet->fieldByName("ico")->getIndex(), Ascending);
  prijemceDataSet->sort(options);

  DataSets::SortOptions options2;
  options2.addOption(subjektyDataSet->fieldByName("IČO_num")->getIndex(),
                     Ascending);
  subjektyDataSet->sort(options2);

  Logger::log(Debug, "Sorted", true);

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
  Logger::log(Info, "Total intersection count: " + std::to_string(cnt), true);

}

void checkDupl() {
  auto prov = new DataProviders::CsvReader(
      csvPath + "dotaceTitul_rozpoctovaSkladbaPolozka.csv", ",");
  auto dataset = new DataSets::MemoryDataSet("tmp");
  dataset->setDataProvider(prov);
  dataset->setFieldTypes({StringValue, StringValue});
  dataset->open();

  auto field = dataset->fieldByIndex(0);

  DataSets::SortOptions options;
  options.addOption(0, Ascending);
  dataset->sort(options);

  std::string last = "";
  int cnt = 0, cnt2 = 0, total = 0;
  bool logged = false;
  while (!dataset->eof()) {
    total++;
    if (Utilities::compareString(field->getAsString(), last) == 0) {
      if (!logged) {
        //Logger::log(Warning, "Duplicate: " + last);
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

  Logger::log(Warning, "Record count: " + std::to_string(total));
  Logger::log(Warning, "Duplicate count: " + std::to_string(cnt2));
  Logger::log(Warning, "Unique duplicate count: " + std::to_string(cnt));
  Logger::log(Warning,
              "Avg on duplicates: " + std::to_string(cnt2 / (double) cnt));

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

int main(int argc, char **argv) {
  writerTest();
  for (auto i = 0; i < 1; ++i) {
    Logger::startTime();
    //iterExample();
    //checkDupl();
    //dominikKontrola();
    //countIntersection();
    Logger::endTime();
    Logger::printElapsedTime();
  }
  return 0;
  Logger::startTime();
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

  Logger::log(Debug, "Providers prepared", true);
  printMemoryUsageMacOS();

  auto dotaceDataSet = new DataSets::MemoryDataSet("dotace");
  dotaceDataSet->setDataProvider(dotaceProvider);
  dotaceDataSet->setFieldTypes({StringValue, StringValue, StringValue,
                                StringValue, StringValue, StringValue});
  dotaceDataSet->open();
  Logger::log(Debug, "Dotace loaded", true);
  printMemoryUsageMacOS();

  auto rozhodnutiDataSet = new DataSets::MemoryDataSet("rozhodnuti");
  rozhodnutiDataSet->setDataProvider(rozhodnutiProvider);
  rozhodnutiDataSet->setFieldTypes({StringValue, StringValue, StringValue});
  rozhodnutiDataSet->open();
  Logger::log(Debug, "Rozhodnuti loaded", true);
  printMemoryUsageMacOS();

  DataWorkers::DataSetMerger merger;
  merger.addDataSet(dotaceDataSet);
  merger.addDataSet(rozhodnutiDataSet);

  auto dotace_rozhodnutiDataSet =
      merger.mergeDataSets("dotace", "rozhodnuti", "idDotace", "idDotace");
  Logger::log(Debug, "Merged dotace, rozhodnuti", true);
  printMemoryUsageMacOS();

  merger.removeDataSet("dotace");
  merger.removeDataSet("rozhodnuti");
  delete dotaceDataSet;
  delete rozhodnutiDataSet;

  auto obdobiDataSet = new DataSets::MemoryDataSet("obdobi");
  obdobiDataSet->setDataProvider(obdobiProvider);
  obdobiDataSet->setFieldTypes({StringValue, StringValue, CurrencyValue,
                                CurrencyValue, StringValue, StringValue});
  obdobiDataSet->open();
  Logger::log(Debug, "Obdobi loaded", true);
  printMemoryUsageMacOS();

  merger.addDataSet(dotace_rozhodnutiDataSet);
  merger.addDataSet(obdobiDataSet);

  auto dotace_rozhodnuti_obdobiDataSet =
      merger.mergeDataSets("dotace_rozhodnuti",
                           "obdobi",
                           "idRozhodnuti",
                           "idRozhodnuti");
  Logger::log(Debug, "Merged dotace_rozhodnuti, obdobi", true);
  printMemoryUsageMacOS();

  merger.removeDataSet("dotace_rozhodnuti");
  merger.removeDataSet("obdobi");
  delete obdobiDataSet;
  delete dotace_rozhodnutiDataSet;

  auto dataWorker =
      new DataWorkers::MemoryDataWorker(dotace_rozhodnuti_obdobiDataSet);

  auto prijemceDataSet = new DataSets::MemoryDataSet("prijemce");
  prijemceDataSet->setDataProvider(prijemceProvider);
  prijemceDataSet->setFieldTypes({StringValue, IntegerValue, StringValue});
  prijemceDataSet->open();
  Logger::log(Debug, "Prijemce loaded", true);
  printMemoryUsageMacOS();

  auto subjektyDataSet = new DataSets::MemoryDataSet("subjekty");
  subjektyDataSet->setDataProvider(subjektyProvider);
  subjektyDataSet->setFieldTypes({IntegerValue, IntegerValue, IntegerValue,
                                  StringValue, StringValue, StringValue,
                                  StringValue});
  subjektyDataSet->open();
  Logger::log(Debug, "Subjekty loaded", true);
  printMemoryUsageMacOS();

  merger.addDataSet(subjektyDataSet);
  merger.addDataSet(prijemceDataSet);
  auto prijemce_subjektyDataSet =
      merger.mergeDataSets("prijemce", "subjekty", "ico", "IČO_num");
  Logger::log(Debug, "Merged prijemce, subjekty", true);
  printMemoryUsageMacOS();

  delete subjektyDataSet;
  delete prijemceDataSet;

  auto operacniProgramProvider =
      new DataProviders::CsvReader(csvPath + operacniProgramCSVName, ",");
  auto operacniProgramDataSet = new DataSets::MemoryDataSet("operacniProgram");
  operacniProgramDataSet->setDataProvider(operacniProgramProvider);
  operacniProgramDataSet->setFieldTypes({StringValue, StringValue});
  operacniProgramDataSet->open();
  Logger::log(Debug, "operacniProgram loaded", true);
  printMemoryUsageMacOS();

  auto grantoveSchemaProvider =
      new DataProviders::CsvReader(csvPath + grantoveSchemaCSVName, ",");
  auto grantoveSchemaDataSet = new DataSets::MemoryDataSet("grantoveSchema");
  grantoveSchemaDataSet->setDataProvider(grantoveSchemaProvider);
  grantoveSchemaDataSet->setFieldTypes({StringValue, StringValue});
  grantoveSchemaDataSet->open();
  Logger::log(Debug, "grantoveSchema loaded", true);
  printMemoryUsageMacOS();

  auto dotaceTitulProvider =
      new DataProviders::CsvReader(csvPath + dotacniTitulCSVName, ",");
  auto dotaceTitulDataSet = new DataSets::MemoryDataSet("dotaceTitul");
  dotaceTitulDataSet->setDataProvider(dotaceTitulProvider);
  dotaceTitulDataSet->setFieldTypes({StringValue, StringValue});
  dotaceTitulDataSet->open();
  Logger::log(Debug, "dotaceTitul loaded", true);
  printMemoryUsageMacOS();

  auto poskytovatelDotaceProvider =
      new DataProviders::CsvReader(csvPath + poskytovatelDotaceCSVName, ",");
  auto poskytovatelDotaceDataSet =
      new DataSets::MemoryDataSet("poskytovatelDotace");
  poskytovatelDotaceDataSet->setDataProvider(poskytovatelDotaceProvider);
  poskytovatelDotaceDataSet->setFieldTypes({StringValue, StringValue});
  poskytovatelDotaceDataSet->open();
  Logger::log(Debug, "poskytovatelDotace loaded", true);
  printMemoryUsageMacOS();

  dataWorker->addDataSet(prijemce_subjektyDataSet);
  dataWorker->addDataSet(operacniProgramDataSet);
  dataWorker->addDataSet(grantoveSchemaDataSet);
  dataWorker->addDataSet(dotaceTitulDataSet);
  dataWorker->addDataSet(poskytovatelDotaceDataSet);
  Logger::log(Debug, "Add datasets to data worker", true);
  printMemoryUsageMacOS();

  auto dataWriter = new DataWriters::CsvWriter(outPath + "all.csv", ",");
  std::string query = QUERY;
  dataWorker->writeResult(*dataWriter,
                          query);

  Logger::log(Debug, "Written query: " + query, true);
  printMemoryUsageMacOS();

  delete dataWriter;
  dataWriter = new DataWriters::CsvWriter(outPath + "all_agr.csv", ",");
  query = QUERY_AGR;
  dataWorker->writeResult(*dataWriter,
                          query);
  Logger::log(Debug, "Written query: " + query, true);
  printMemoryUsageMacOS();
  delete dataWriter;

  dataWriter = new DataWriters::CsvWriter(outPath + "2017.csv", ",");
  query = QUERY_2017;
  dataWorker->writeResult(*dataWriter,
                          query);
  Logger::log(Debug, "Written query: " + query, true);
  printMemoryUsageMacOS();
  delete dataWriter;

  dataWriter = new DataWriters::CsvWriter(outPath + "2017_agr.csv", ",");
  query = QUERY_AGR_2017;
  dataWorker->writeResult(*dataWriter,
                          query);
  Logger::log(Debug, "Written query: " + query, true);
  printMemoryUsageMacOS();
  delete dataWriter;

  delete dataWorker;
  delete subjektyProvider;
  printMemoryUsageMacOS();

  delete operacniProgramDataSet;
  delete grantoveSchemaDataSet;
  delete dotaceTitulDataSet;
  delete poskytovatelDotaceDataSet;
  Logger::log(Debug, "Done", true);
  printMemoryUsageMacOS();

  Logger::endTime();
  Logger::printElapsedTime();
  return 0;
}
