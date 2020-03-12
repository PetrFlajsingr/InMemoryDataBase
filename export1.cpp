//
// Created by petr on 10/3/19.
//

#include "LoadingUtils.h"
#include "io/logger.h"
#include <CsvWriter.h>
#include <EmptyWriter.h>
#include <MemoryDataBase.h>
#include <MemoryDataSet.h>
#include <XlntReader.h>
#include <XlntWriter.h>
#include <XlsxIOReader.h>
#include <XlsxIOWriter.h>
#include <include/fmt/format.h>
#include <io/print.h>
#include <memory>
#include <various/isin.h>

using namespace std::string_literals;
const auto notAvailable = "není k dispozici"s;
Logger logger{std::cout};

const auto csvPath = "/home/petr/Desktop/muni/"s;
const auto outPath = csvPath + "out/";

const auto resPath = csvPath + "res/res.csv";
const auto rzpPath = csvPath + "rzp/rzp.csv";
const auto justicePath = csvPath + "or.justice/justice.csv";
const auto copniPath = csvPath + "CSU2019_copni.xlsx";
const auto geoPath = csvPath + "geo.csv";

std::shared_ptr<DataSets::MemoryDataSet> shrinkRzp(const std::shared_ptr<DataSets::BaseDataSet> &rzp) {
  DataSets::SortOptions sortOptions;
  sortOptions.addOption(rzp->fieldByName("ICO"), SortOrder::Ascending);
  rzp->sort(sortOptions);

  auto result = std::make_shared<DataSets::MemoryDataSet>("rzp_shrinked");
  result->openEmpty(rzp->getFieldNames(), {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                           ValueType::String, ValueType::String});

  rzp->resetBegin();
  int lastICO = 0;

  auto icoField = dynamic_cast<DataSets::IntegerField *>(rzp->fieldByName("ICO"));
  auto linkField = rzp->fieldByName("ZIVNOSTENSKE_OPRAVNENI_ODKAZ");
  auto fields = rzp->getFields();

  auto resultIcoField = dynamic_cast<DataSets::IntegerField *>(result->fieldByName("ICO"));
  auto resultFields = result->getFields();
  while (rzp->next()) {
    if (icoField->getAsInteger() != lastICO) {
      lastICO = icoField->getAsInteger();
      result->append();
      resultIcoField->setAsInteger(lastICO);

      for (auto iter1 = fields.begin() + 1, iter2 = resultFields.begin() + 1; iter1 != fields.end(); ++iter1, ++iter2) {
        (*iter2)->setAsString((*iter1)->getAsString());
      }
    } else {
      for (auto iter1 = fields.begin() + 1, iter2 = resultFields.begin() + 1; iter1 != fields.end() - 1;
           ++iter1, ++iter2) {
        if (*iter1 != linkField) {
          (*iter2)->setAsString((*iter2)->getAsString() + "|" + (*iter1)->getAsString());
        }
      }
    }
  }

  return result;
}

std::shared_ptr<DataSets::MemoryDataSet> distinctCopniCodes(DataSets::BaseDataSet &ds) {
  auto result = std::make_shared<DataSets::MemoryDataSet>("copni2");
  result->openEmpty(ds.getFieldNames(),
                    {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String});
  DataSets::SortOptions sortOptions;
  sortOptions.addOption(ds.fieldByName("COPNI_codes"), SortOrder::Ascending);
  ds.sort(sortOptions);

  std::vector<std::string> containedCodes;
  ds.resetBegin();
  auto codeField = ds.fieldByName("COPNI_codes");
  auto srcFields = ds.getFields();
  auto dstFields = result->getFields();
  while (ds.next()) {
    if (isIn(codeField->getAsString(), containedCodes)) {
      continue;
    }
    containedCodes.emplace_back(codeField->getAsString());
    result->append();
    for (auto i : MakeRange::range(srcFields.size())) {
      dstFields[i]->setAsString(srcFields[i]->getAsString());
    }
  }
  return result;
}

int main() {
  struct CirkevniData {
    std::string ucel;
    std::string zakladatel;
  };
  std::unordered_map<int, CirkevniData> cirkev;

  {
    DataProviders::XlsxIOReader cirkevReader{csvPath + "MK_cirkevni_organizace.xlsx"};

    for (const auto &row : cirkevReader) {
      CirkevniData data{row[8], row[7]};
      cirkev[Utilities::stringToInt(row[0])] = data;
    }
  }

  logger.setDefaultPrintTime(true);
  DataBase::MemoryDataBase db("db");

  db.addTable(createDataSetFromFile("rzp", FileSettings::Csv(rzpPath),
                                    {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String}));

  db.addTable(shrinkRzp(db.tableByName("rzp")->dataSet));

  db.addTable(createDataSetFromFile("res", FileSettings::Csv(resPath, ','),
                                    {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String,
                                     ValueType::String}));

  db.addTable(createDataSetFromFile("justice", FileSettings::Csv(justicePath),
                                    {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String}));
  db.addTable(createDataSetFromFile("geo", FileSettings::Csv(geoPath),
                                    {ValueType::Integer, ValueType::String, ValueType::String}));

  db.addTable(distinctCopniCodes(*createDataSetFromFile(
      "copni2", FileSettings::Xlsx(copniPath, "COPNImetod"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String})));
  db.addTable(createDataSetFromFile(
      "copni1", FileSettings::Xlsx(copniPath, "CSU_copni"),
      {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String}));

  const auto copniQuery =
      R"(select copni1.ICO, copni2."Popis cinnosti", copni2."Oblast cinnosti"
from copni1
join copni2 on copni1.COPNI = copni2.COPNI_codes;)";
  db.addTable(db.execSimpleQuery(copniQuery, false, "copni")->dataSet->toDataSet());
  db.removeTable("copni1");
  db.removeTable("copni2");

  const std::string queryFin =
      R"(SELECT res.ICO_number,
res.ICO,
res.NAZEV,
res.PRAVNI_FORMA,
res.VELIKOSTNI_KATEGORIE,
res.ADRESA,
res.KRAJ,
res.OKRES,
res.OBEC,
res.OBEC_OKRES,
res.OBEC_ICOB,
res.DATUM_VZNIKU,
res.ROK_VZNIKU,
res.DATUM_LIKVIDACE,
res.ROK_LIKVIDACE,
res.INSTITUCE_V_LIKVIDACI,
copni."Popis cinnosti" as COPNI_cinnost,
copni."Oblast cinnosti" AS COPNI_obor,
justice.POBOCNE_SPOLKY AS AKTUALNI_INFORMACE_VE_VEREJNEM_REJSTRIKU,
justice.DATUM_ZAPISU,
justice.KAPITAL as NADACNI_KAPITAL,
justice.STATUTARNI_ORGAN,
justice.ZAKLADATEL,
justice.POBOCNE_SPOLKY_POCET,
justice.UCEL as UCEL_POPIS,
res.NACE_cinnost,
res.NACE_kod,
rzp_shrinked.PREDMET_CINNOSTI,
rzp_shrinked.ZIVNOSTENSKE_OPRAVNENI_OBOR,
rzp_shrinked.ZIVNOSTENSKE_OPRAVNENI,
rzp_shrinked.ZIVNOSTENSKE_OPRAVNENI_ODKAZ,
rzp_shrinked.MA_ZIVNOSTENSKE_OPRAVNENI,
geo.x as Y,
geo.y as X
FROM res
left join justice on res.ICO_number = justice.ICO
left join rzp_shrinked on res.ICO_number = rzp_shrinked.ICO
left join copni on res.ICO_number = copni.ICO
left join geo on res.ICO_number = geo.ICO;)";
  auto res = db.execSimpleQuery(queryFin, false, "res");

  res->dataSet->resetBegin();
  auto resDS = res->dataSet->toDataSet();
  auto zanikField = resDS->fieldByName("DATUM_LIKVIDACE");
  auto kapitalField = resDS->fieldByName("NADACNI_KAPITAL");
  auto zakladatelField = resDS->fieldByName("ZAKLADATEL");
  resDS->resetBegin();
  while (resDS->next()) {
    if (zanikField->getAsString().empty()) {
      zanikField->setAsString("Aktivní");
    }
    if (kapitalField->getAsString().empty()) {
      kapitalField->setAsString(notAvailable);
    }
    if (zakladatelField->getAsString().empty()) {
      zakladatelField->setAsString(notAvailable);
    }
  }

  DataWriters::CsvWriter writerCsv{"/home/petr/Desktop/muni/neziskovky.csv"};
  DataWriters::EmptyWriter writerXlsx{"/home/petr/Desktop/muni/neziskovky.xlsx"};

  DataWriters::CsvWriter writerCsv_aktivni{"/home/petr/Desktop/muni/neziskovky_aktivni.csv"};
  DataWriters::EmptyWriter writerXlsx_aktivni{"/home/petr/Desktop/muni/neziskovky_aktivni.xlsx"};
  writerCsv.setAddQuotes(true);
  writerCsv_aktivni.setAddQuotes(true);

  auto header = resDS->getFieldNames();
  header.emplace_back("download_period");

  writerCsv.writeHeader(header);
  writerXlsx.writeHeader(header);
  writerCsv_aktivni.writeHeader(header);
  writerXlsx_aktivni.writeHeader(header);

  resDS->resetBegin();
  auto fields = resDS->getFields();
  std::vector<std::string> row;
  row.reserve(header.size());

  const auto currentTime = std::chrono::system_clock::now();
  auto now_t = std::chrono::system_clock::to_time_t(currentTime);
  struct tm *parts = std::localtime(&now_t);
  const auto year = 1900 + parts->tm_year;
  const auto month = 1 + parts->tm_mon;
  const auto downloadPeriodData = fmt::format("{:04d}-{:02d}", year, month);

  auto fieldVLikvidaci = resDS->fieldByName("INSTITUCE_V_LIKVIDACI");
  auto fieldUcel = resDS->fieldByName("UCEL_POPIS");
  auto fieldZakladatel = resDS->fieldByName("ZAKLADATEL");
  auto fieldMaZO = resDS->fieldByName("MA_ZIVNOSTENSKE_OPRAVNENI");
  auto fieldIco = dynamic_cast<DataSets::IntegerField *>(resDS->fieldByName("ICO_number"));
  while (resDS->next()) {
    if (fieldUcel->getAsString().empty() || fieldUcel->getAsString() == notAvailable) {
      if (cirkev.contains(fieldIco->getAsInteger())) {
        fieldUcel->setAsString(cirkev[fieldIco->getAsInteger()].ucel);
      }
    }
    if (fieldZakladatel->getAsString().empty() || fieldZakladatel->getAsString() == notAvailable) {
      if (cirkev.contains(fieldIco->getAsInteger())) {
        fieldZakladatel->setAsString(cirkev[fieldIco->getAsInteger()].zakladatel);
      }
    }
    if (fieldMaZO->getAsString() == notAvailable) {
      fieldMaZO->setAsString("NE");
    }
    std::transform(fields.begin(), fields.end(), std::back_inserter(row),
                   [](auto &field) { return Utilities::defaultForEmpty(field->getAsString(), notAvailable); });
    row[1] = fmt::format("{:08d}", fieldIco->getAsInteger());
    row.emplace_back(downloadPeriodData);
    writerCsv.writeRecord(row);
    writerXlsx.writeRecord(row);
    if (fieldVLikvidaci->getAsString() == "aktivní") {
      writerCsv_aktivni.writeRecord(row);
      writerXlsx_aktivni.writeRecord(row);
    }
    row.clear();
  }

  logger.log<LogLevel::Debug, true>("Result rows: ", resDS->getCurrentRecord());
}