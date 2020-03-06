//
// Created by petr on 10/3/19.
//

#include <Logger.h>
#include <MemoryDataBase.h>
#include <MemoryDataSet.h>
#include <XlntWriter.h>
#include <XlsxIOReader.h>
#include <XlsxIOWriter.h>
#include <include/fmt/format.h>
#include <memory>
#include "LoadingUtils.h"

using namespace std::string_literals;

auto &logger = Logger<true>::GetInstance();

const std::string csvPath = "/home/petr/Desktop/muni/";
const std::string outPath = csvPath + "out/";
const std::string subjektyCSVName = "NNO.xlsx";
const std::string justiceCSV = "/home/petr/Desktop/muni/or.justice/justice.csv";
const std::string rzpCSV = "/home/petr/Desktop/muni/rzp/rzp.csv";
const std::string nnoDopl = "/home/petr/Desktop/muni/NNO_doplneni.xlsx";
const std::string velkatCSV = "/home/petr/Desktop/muni/velikostni_kategorie.csv";

std::shared_ptr<DataSets::MemoryDataSet> shrinkRzp(const std::shared_ptr<DataSets::BaseDataSet> &rzp) {
  DataSets::SortOptions sortOptions;
  sortOptions.addOption(rzp->fieldByName("ICO"), SortOrder::Ascending);
  rzp->sort(sortOptions);

  auto result = std::make_shared<DataSets::MemoryDataSet>("rzp_shrinked");
  result->openEmpty(rzp->getFieldNames(),
                    {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String, ValueType::String});

  rzp->resetBegin();
  int lastICO = 0;

  auto icoField = dynamic_cast<DataSets::IntegerField *>(rzp->fieldByName("ICO"));
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
      for (auto iter1 = fields.begin() + 1, iter2 = resultFields.begin() + 1; iter1 != fields.end(); ++iter1, ++iter2) {
        (*iter2)->setAsString((*iter2)->getAsString() + ";" + (*iter1)->getAsString());
      }
    }
  }

  return result;
}

int main() {
  DataBase::MemoryDataBase db("db");
  db.addTable(createDataSetFromFile("nno", FileSettings::Xlsx(csvPath + subjektyCSVName),
                                    {ValueType::Integer, ValueType::Integer, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String}));

  db.addTable(createDataSetFromFile("velkat", {FileSettings::Type::csv, velkatCSV, ','},
                                    {ValueType::String, ValueType::String}));

  db.addTable(createDataSetFromFile("justice", {FileSettings::Type::csv, justiceCSV, ','},
                                    {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String}));

  db.addTable(createDataSetFromFile(
      "rzp", {FileSettings::Type::csv, rzpCSV, ','},
      {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String, ValueType::String}));

  db.addTable(
      createDataSetFromFile("nace", FileSettings::Xlsx(nnoDopl, "NACE_kódy"), {ValueType::String, ValueType::Integer}));

  db.addTable(createDataSetFromFile(
      "copni1", FileSettings::Xlsx(csvPath + "CSU2019_copni.xlsx", "CSU_copni"),
      {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::Integer}));
  db.addTable(createDataSetFromFile(
      "copni2", FileSettings::Xlsx(csvPath + "CSU2019_copni.xlsx", "COPNImetod"),
      {ValueType::String, ValueType::Integer, ValueType::String, ValueType::String, ValueType::String}));

  const auto copniQuery =
      "select copni1.ICO, copni2.\"Popis cinnosti\" from copni1 join copni2 on copni1.COPNI = copni2.COPNI_codes;";
  auto copniDesc = db.execSimpleQuery(copniQuery, false, "copni");
  db.addTable(copniDesc->dataSet->toDataSet());
  db.removeTable("copni1");
  db.removeTable("copni2");

  std::unordered_map<std::string, std::string> kategorie;
  auto velkat = db.tableByName("velkat");
  velkat->dataSet->resetBegin();
  while (velkat->dataSet->next()) {
    kategorie[velkat->dataSet->fieldByName("TEXT")->getAsString()] =
        fmt::format("{}) ", velkat->dataSet->fieldByName("velikostni_kategorie_index")->getAsString());
  }

  {
    auto subjekty = db.tableByName("nno")->dataSet;
    subjekty->resetBegin();
    auto katField = subjekty->fieldByName("Velikostní_kategorie");
    while (subjekty->next()) {
      auto newKat = kategorie[katField->getAsString()];
      katField->setAsString(newKat + katField->getAsString());
    }
    subjekty->resetBegin();
  }

  const std::string query =
      "SELECT rzp.*, nace.Kód_NACE from rzp join nace on rzp.ZIVNOSTENSKE_OPRAVNENI_OBOR = nace.cinnost_NACE;";
  auto tmp = db.execSimpleQuery(query, false, "rzp2");
  db.addTable(shrinkRzp(tmp->dataSet));

  const std::string queryFin =
      "SELECT nno.ICOnum, "
      "nno.IC_ORGANIZACE, "
      "nno.NAZEV, "
      "nno.PRAVNI_FORMA, "
      "nno.Velikostní_kategorie, "
      "nno.Adresa, "
      "nno.KRAJ, "
      "nno.OKRES, "
      "nno.OBEC, "
      "nno.OBEC_ICOB, "
      "nno.DATUM_VZNIKU, "
      "nno.DATUM_LIKVIDACE, "
      "nno.INSTITUCE_V_LIKVIDACI, "
      "copni.\"Popis cinnosti\" as UCEL_PREDMET_CINNOSTI, "
      "nno.KATEGORIZACE_CINNOSTI, "
      "nno.X, "
      "nno.Y, justice.DATUM_ZAPISU, justice.NADACNI_KAPITAL, justice.ZAKLADATEL, "
      "justice.STATUTARNI_ORGAN, justice.POBOCNE_SPOLKY, justice.UCETNI_UZAVERKA, justice.STANOVY_A_DALSI_DOKUMENTY, "
      "justice.POBOCNE_SPOLKY_ODKAZ, "
      "rzp_shrinked.ZIVNOSTENSKE_OPRAVNENI_OBOR, rzp_shrinked.PREDMET_CINNOSTI, "
      "rzp_shrinked.ZIVNOSTENSKE_OPRAVNENI, rzp_shrinked.Kód_NACE "
      "FROM nno join justice on nno.ICOnum = justice.ICO "
      "left join rzp_shrinked on nno.ICOnum = rzp_shrinked.ICO "
      "left join copni on nno.ICOnum = copni.ICO "
      "where nno.INSTITUCE_V_LIKVIDACI = \"aktivní\";";
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
      kapitalField->setAsString("není k dispozici");
    }
    if (zakladatelField->getAsString().empty()) {
      zakladatelField->setAsString("není k dispozici");
    }
  }

  DataWriters::XlntWriter writer{"/home/petr/Desktop/muni/export1.xlsx"};
  // writer.setAddQuotes(true);
  auto header = resDS->getFieldNames();
  header.insert(header.begin() + 24, "POBOCNE_SPOLKY_POCET");
  header.insert(header.begin() + 26, "UCEL_POPIS");

  header.emplace_back("download_period");
  writer.writeHeader(header);
  resDS->resetBegin();
  auto fields = resDS->getFields();
  std::vector<std::string> row;
  row.reserve(header.size() + 1);

  const auto currentTime = std::chrono::system_clock::now();
  auto now_t = std::chrono::system_clock::to_time_t(currentTime);
  struct tm *parts = std::localtime(&now_t);
  const auto year = 1900 + parts->tm_year;
  const auto month = 1 + parts->tm_mon;
  const auto downloadPeriodData = fmt::format("{:04d}-{:02d}", year, month);

  while (resDS->next()) {
    std::transform(fields.begin(), fields.end(), std::back_inserter(row),
                   [](auto &field) { return field->getAsString(); });
    row.emplace_back(downloadPeriodData);
    row.insert(row.begin() + 24, "0");
    row.insert(row.begin() + 26, "<placeholder>");
    writer.writeRecord(row);
    row.clear();
  }

  // writer.writeDataSet(*resDS);

  logger.log<LogLevel::Debug, true>("Result rows: ", resDS->getCurrentRecord());
}