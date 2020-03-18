//
// Created by petr on 3/14/20.
//
#include "LoadingUtils.h"
#include "fmt/format.h"
#include <CsvWriter.h>
#include <MemoryDataBase.h>
#include <io/print.h>
#include <string>
#include <various/isin.h>

using namespace std::string_literals;

const auto csvPath = "/home/petr/Desktop/muni/"s;
const auto outPath = csvPath + "out/";
const auto nrzpsPath = csvPath + "nrzps/narodni-registr-poskytovatelu-zdravotnich-sluzeb.csv";
const auto resPath = csvPath + "res/res_full.csv";
const auto notAvailable = "není k dispozici"s;
const auto justicePath = csvPath + "or.justice/justice.csv";
const auto copniPath = csvPath + "CSU2019_copni.xlsx";
const auto geoPath = csvPath + "geo.csv";
const auto copniSektorPath = csvPath + "INPUT_pf_copni_okres_sektor.xlsx";
const auto rzpPath = csvPath + "rzp/rzp.csv";


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

std::string buildAdresa(const std::string &obec, const std::string &psc, const std::string &ulice,
                        const std::string &cisloDomu) {
  std::string result = fmt::format("{}, {}", obec, psc);
  bool hadUliceOrCastObce = false;
  if (!ulice.empty()) {
    hadUliceOrCastObce = true;
    result += fmt::format(", {}", ulice);
  }
  if (!hadUliceOrCastObce) {
    result += fmt::format(", {}", obec);
  }
  result += fmt::format(" {}", cisloDomu);

  return result;
};

int main() {
  const auto currentTime = std::chrono::system_clock::now();
  auto now_t = std::chrono::system_clock::to_time_t(currentTime);
  struct tm *parts = std::localtime(&now_t);
  const auto year = 1900 + parts->tm_year;
  const auto month = 1 + parts->tm_mon;
  const auto downloadPeriodData = fmt::format("{:04d}-{:02d}", year, month);

  DataBase::MemoryDataBase db{"db"};

  db.addTable(createDataSetFromFile("res", FileSettings::Csv(resPath, ','),
                                    {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String}));

  db.addTable(createDataSetFromFile("rzp", FileSettings::Csv(rzpPath),
                                    {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String}));

  db.addTable(shrinkRzp(db.tableByName("rzp")->dataSet));

  db.addTable(createDataSetFromFile(
      "prav_formy_SEKTOR", FileSettings::Xlsx(copniSektorPath, "prav_formy_SEKTOR"),
      {ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String}));

  db.addTable(createDataSetFromFile(
      "nrzps", FileSettings::Csv(nrzpsPath),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String,  ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String,  ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String,  ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::Integer, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String,  ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String,  ValueType::String,
       ValueType::String, ValueType::String, ValueType::String}));



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

  const auto queryZarizeni = R"(
SELECT
nrzps.NazevZarizeni as NAZEV_ZARIZENI,
nrzps.DruhZarizeni as DRUH_ZARIZENI,
nrzps.OborPece as OBOR_PECE,
nrzps.FormaPece as FORMA_PECE,
nrzps.DruhPece as DRUH_PECE,
nrzps.Ulice as ULICE,
nrzps.CisloDomovniOrientacni as CISLO_DOMU,
nrzps.Obec as OBEC,
nrzps.Psc as PSC,
nrzps.Kraj as KRAJ,
nrzps.Okres as OKRES,
res.NAZEV as NAZEV_POSKYTOVATELE,
res.ICO_number,
res.ICO,
res.ADRESA as ADRESA_POSKYTOVATELE,
res.KRAJ as KRAJ_POSKYTOVATELE,
res.OKRES as OKRES_POSKYTOVATELE,
res.OBEC as OBEC_POSKYTOVATELE,
res.OBEC_OKRES as OBEC_OKRES_POSKYTOVATELE,
res.PRAVNI_FORMA as PRAVNI_FORMA_POSKYTOVATELE,
res.INSTITUCE_V_LIKVIDACI as AKTIVNI
FROM nrzps
JOIN res ON nrzps.Ico = res.ICO_number
ORDER BY res.ICO_number ASC;
)";

  const auto queryPoskytovatel = R"(
SELECT
nrzps.NazevZarizeni as NAZEV_ZARIZENI,
res.NAZEV as NAZEV_POKYSTOVATELE,
res.ICO,
res.ICO_number,
res.VELIKOSTNI_KATEGORIE,
res.ADRESA,
nrzps.PoskytovatelTelefon as TELEFON,
nrzps.PoskytovatelEmail as EMAIL,
nrzps.PoskytovatelWeb as WEB,
res.KRAJ,
res.OKRES,
res.OBEC,
res.OBEC_OKRES,
res.PRAVNI_FORMA,
res.DATUM_VZNIKU,
res.ROK_VZNIKU,
res.INSTITUCE_V_LIKVIDACI as AKTIVNI
FROM nrzps
JOIN res on nrzps.Ico = res.ICO_number
ORDER BY res.ICO_number ASC;
)";

  const auto queryPoskytovatelRES = R"(
SELECT
res.ICO_number,
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
justice.UCEL as POSLANI,
res.NACE_cinnost,
res.NACE_kod,
rzp_shrinked.PREDMET_CINNOSTI,
rzp_shrinked.ZIVNOSTENSKE_OPRAVNENI_OBOR,
rzp_shrinked.ZIVNOSTENSKE_OPRAVNENI,
rzp_shrinked.ZIVNOSTENSKE_OPRAVNENI_ODKAZ,
rzp_shrinked.MA_ZIVNOSTENSKE_OPRAVNENI,
geo.x as Y,
geo.y as X,
nrzps.PoskytovatelTelefon as TELEFON,
nrzps.PoskytovatelEmail as EMAIL,
nrzps.PoskytovatelWeb as WEB,
nrzps.OdbornyZastupce as ODBORNY_ZASTUPCE,
prav_formy_SEKTOR.sub_sektor,
prav_formy_SEKTOR.sektor
FROM nrzps
JOIN res ON nrzps.Ico = res.ICO_number
LEFT JOIN justice on nrzps.Ico = justice.ICO
LEFT JOIN copni on res.ICO_number = copni.ICO
left join rzp_shrinked on res.ICO_number = rzp_shrinked.ICO
left join geo on res.ICO_number = geo.ICO
LEFT JOIN prav_formy_SEKTOR ON res.FORMA = prav_formy_SEKTOR.NNO
ORDER BY res.ICO_number ASC;
)";
  std::unordered_map<int, std::string> pocetZarizeniProICO;
  {
    auto poskytovateleDS = db.execSimpleQuery(queryPoskytovatel, false, "poskytovatel")->dataSet;
    auto justiceDS = db.tableByName("justice")->dataSet;
    {
      DataSets::SortOptions sortOptions;
      sortOptions.addOption(justiceDS->fieldByName("ICO"), SortOrder::Ascending);
      justiceDS->sort(sortOptions);
      justiceDS->resetBegin();
      justiceDS->next();
    }

    DataWriters::CsvWriter writerPoskytovatelAktivni{outPath + "nrzps_org_gis.csv"};
    writerPoskytovatelAktivni.setAddQuotes(true);

    std::vector<std::string> row;
    auto header = poskytovateleDS->getFieldNames();
    header.erase(header.end() - 1);
    header.erase(header.begin());
    header.insert(header.begin() + 3, "POSLANI");
    header.emplace_back("POCET_ZARIZENI");
    auto fields = poskytovateleDS->getFields();
    fields.erase(fields.begin());
    auto aktivniField = poskytovateleDS->fieldByName("AKTIVNI");
    auto icoField = dynamic_cast<DataSets::IntegerField *>(poskytovateleDS->fieldByName("ICO_number"));
    poskytovateleDS->resetBegin();
    int lastICO = 0;
    unsigned int counter = 0;
    writerPoskytovatelAktivni.writeHeader(header);

    auto justiceICOField = dynamic_cast<DataSets::IntegerField *>(justiceDS->fieldByName("ICO"));
    while (poskytovateleDS->next()) {

      if (lastICO != icoField->getAsInteger()) {
        if (lastICO != 0) {
          if (aktivniField->getAsString() == "aktivní") {
            const auto strCounter = std::to_string(counter);
            row.emplace_back(strCounter);
            pocetZarizeniProICO[lastICO] = strCounter;
            writerPoskytovatelAktivni.writeRecord(row);
          }
        }
        row.clear();
        std::transform(fields.begin(), fields.end() - 1, std::back_inserter(row), [](const auto &field) {
          return Utilities::defaultForEmpty(field->getAsString(), notAvailable);
        });
        while (!justiceDS->isEnd() && justiceICOField->getAsInteger() < lastICO) {
          justiceDS->next();
        }
        if (justiceICOField->getAsInteger() == lastICO) {
          row.insert(row.begin() + 3, justiceDS->fieldByName("UCEL")->getAsString());
        } else {
          row.insert(row.begin() + 3, notAvailable);
        }
        counter = 0;
        lastICO = icoField->getAsInteger();

      }
      ++counter;
    }
  }
  {
    DataWriters::CsvWriter writerOrgFull{outPath + "nrzps_org_full.csv"};
    writerOrgFull.setAddQuotes(true);
    writerOrgFull.writeHeader({"ICO_number",
                               "ICO",
                               "NAZEV",
                               "PRAVNI_FORMA",
                               "VELIKOSTNI_KATEGORIE",
                               "ADRESA",
                               "KRAJ",
                               "OKRES",
                               "OBEC",
                               "OBEC_OKRES",
                               "OBEC_ICOB",
                               "DATUM_VZNIKU",
                               "ROK_VZNIKU",
                               "DATUM_LIKVIDACE",
                               "ROK_LIKVIDACE",
                               "INSTITUCE_V_LIKVIDACI",
                               "COPNI_cinnost",
                               "COPNI_obor",
                               "AKTUALNI_INFORMACE_VE_VEREJNEM_REJSTRIKU",
                               "DATUM_ZAPISU",
                               "NADACNI_KAPITAL",
                               "STATUTARNI_ORGAN",
                               "ZAKLADATEL",
                               "POSLANI",
                               "POCET_ZARIZENI",
                               "NACE_cinnost",
                               "NACE_kod",
                               "PREDMET_CINNOSTI",
                               "ZIVNOSTENSKE_OPRAVNENI_OBOR",
                               "ZIVNOSTENSKE_OPRAVNENI",
                               "ZIVNOSTENSKE_OPRAVNENI_ODKAZ",
                               "MA_ZIVNOSTENSKE_OPRAVNENI",
                               "Y",
                               "X",
                               "download_period",
                               "TELEFON",
                               "EMAIL",
                               "WEB",
                               "ODBORNY_ZASTUPCE",
                               "sub_sektor",
                               "sektor"});
    std::vector<std::string> row;
    auto queryResultDS = db.execSimpleQuery(queryPoskytovatelRES, false, "nrzps_org_full")->dataSet;
    auto field_ICO_number = dynamic_cast<DataSets::IntegerField *>(queryResultDS->fieldByName("ICO_number"));
    auto field_ICO = queryResultDS->fieldByName("ICO");
    auto field_NAZEV = queryResultDS->fieldByName("NAZEV");
    auto field_PRAVNI_FORMA = queryResultDS->fieldByName("PRAVNI_FORMA");
    auto field_VELIKOSTNI_KATEGORIE = queryResultDS->fieldByName("VELIKOSTNI_KATEGORIE");
    auto field_ADRESA = queryResultDS->fieldByName("ADRESA");
    auto field_KRAJ = queryResultDS->fieldByName("KRAJ");
    auto field_OKRES = queryResultDS->fieldByName("OKRES");
    auto field_OBEC = queryResultDS->fieldByName("OBEC");
    auto field_OBEC_OKRES = queryResultDS->fieldByName("OBEC_OKRES");
    auto field_OBEC_ICOB = queryResultDS->fieldByName("OBEC_ICOB");
    auto field_DATUM_VZNIKU = queryResultDS->fieldByName("DATUM_VZNIKU");
    auto field_ROK_VZNIKU = queryResultDS->fieldByName("ROK_VZNIKU");
    auto field_DATUM_LIKVIDACE = queryResultDS->fieldByName("DATUM_LIKVIDACE");
    auto field_ROK_LIKVIDACE = queryResultDS->fieldByName("ROK_LIKVIDACE");
    auto field_INSTITUCE_V_LIKVIDACI = queryResultDS->fieldByName("INSTITUCE_V_LIKVIDACI");
    auto field_COPNI_cinnost = queryResultDS->fieldByName("COPNI_cinnost");
    auto field_COPNI_obor = queryResultDS->fieldByName("COPNI_obor");
    auto field_AKTUALNI_INFORMACE_VE_VEREJNEM_REJSTRIKU =
        queryResultDS->fieldByName("AKTUALNI_INFORMACE_VE_VEREJNEM_REJSTRIKU");
    auto field_DATUM_ZAPISU = queryResultDS->fieldByName("DATUM_ZAPISU");
    auto field_NADACNI_KAPITAL = queryResultDS->fieldByName("NADACNI_KAPITAL");
    auto field_STATUTARNI_ORGAN = queryResultDS->fieldByName("STATUTARNI_ORGAN");
    auto field_ZAKLADATEL = queryResultDS->fieldByName("ZAKLADATEL");
    auto field_POSLANI = queryResultDS->fieldByName("POSLANI");
    auto field_NACE_cinnost = queryResultDS->fieldByName("NACE_cinnost");
    auto field_NACE_kod = queryResultDS->fieldByName("NACE_kod");
    auto field_PREDMET_CINNOSTI = queryResultDS->fieldByName("PREDMET_CINNOSTI");
    auto field_ZIVNOSTENSKE_OPRAVNENI_OBOR = queryResultDS->fieldByName("ZIVNOSTENSKE_OPRAVNENI_OBOR");
    auto field_ZIVNOSTENSKE_OPRAVNENI = queryResultDS->fieldByName("ZIVNOSTENSKE_OPRAVNENI");
    auto field_ZIVNOSTENSKE_OPRAVNENI_ODKAZ = queryResultDS->fieldByName("ZIVNOSTENSKE_OPRAVNENI_ODKAZ");
    auto field_MA_ZIVNOSTENSKE_OPRAVNENI = queryResultDS->fieldByName("MA_ZIVNOSTENSKE_OPRAVNENI");
    auto field_Y = queryResultDS->fieldByName("Y");
    auto field_X = queryResultDS->fieldByName("X");
    auto field_TELEFON = queryResultDS->fieldByName("TELEFON");
    auto field_EMAIL = queryResultDS->fieldByName("EMAIL");
    auto field_WEB = queryResultDS->fieldByName("WEB");
    auto field_ODBORNY_ZASTUPCE = queryResultDS->fieldByName("ODBORNY_ZASTUPCE");
    auto field_sub_sektor = queryResultDS->fieldByName("sub_sektor");
    auto field_sektor = queryResultDS->fieldByName("sektor");

    queryResultDS->resetBegin();

    int lastICO = 0;
    while (queryResultDS->next()) {
      if (field_ICO_number->getAsInteger() == lastICO) {
        continue;
      }
      lastICO = field_ICO_number->getAsInteger();
      row.clear();
      row.emplace_back(Utilities::defaultForEmpty(field_ICO_number->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ICO->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_NAZEV->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_PRAVNI_FORMA->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_VELIKOSTNI_KATEGORIE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ADRESA->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_KRAJ->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OKRES->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OBEC->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OBEC_OKRES->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OBEC_ICOB->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_DATUM_VZNIKU->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ROK_VZNIKU->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_DATUM_LIKVIDACE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ROK_LIKVIDACE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_INSTITUCE_V_LIKVIDACI->getAsString(), notAvailable));

      auto copniCinnost = field_COPNI_cinnost->getAsString();
      if (copniCinnost.empty() || copniCinnost == notAvailable) {
        copniCinnost = "Zdravotnictví";
      }
      auto copniObor = field_COPNI_obor->getAsString();
      if (copniObor.empty() || copniObor == notAvailable) {
        copniObor = "Zdravotnictví";
      }
      row.emplace_back(copniCinnost);
      row.emplace_back(copniObor);
      row.emplace_back(
          Utilities::defaultForEmpty(field_AKTUALNI_INFORMACE_VE_VEREJNEM_REJSTRIKU->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_DATUM_ZAPISU->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_NADACNI_KAPITAL->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_STATUTARNI_ORGAN->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ZAKLADATEL->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_POSLANI->getAsString(), notAvailable));
      if (pocetZarizeniProICO.contains(field_ICO_number->getAsInteger())) {
        row.emplace_back(pocetZarizeniProICO[field_ICO_number->getAsInteger()]);
      } else {
        row.emplace_back(notAvailable);
      };
      row.emplace_back(Utilities::defaultForEmpty(field_NACE_cinnost->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_NACE_kod->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_PREDMET_CINNOSTI->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ZIVNOSTENSKE_OPRAVNENI_OBOR->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ZIVNOSTENSKE_OPRAVNENI->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ZIVNOSTENSKE_OPRAVNENI_ODKAZ->getAsString(), notAvailable));
      auto maZivnostOpr = field_MA_ZIVNOSTENSKE_OPRAVNENI->getAsString();
      if (maZivnostOpr == notAvailable) {
        maZivnostOpr = "NE";
      }
      row.emplace_back(Utilities::defaultForEmpty(maZivnostOpr, notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_Y->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_X->getAsString(), notAvailable));
      row.emplace_back(downloadPeriodData);
      row.emplace_back(Utilities::defaultForEmpty(field_TELEFON->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_EMAIL->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_WEB->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ODBORNY_ZASTUPCE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_sub_sektor->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_sektor->getAsString(), notAvailable));
      writerOrgFull.writeRecord(row);
    }
  }
  {
    DataWriters::CsvWriter writerZarizeni{outPath + "nrzps_zar_full.csv"};
    writerZarizeni.setAddQuotes(true);
    DataWriters::CsvWriter writerZarizeniAktivni{outPath + "nrzps_zar_gis.csv"};
    writerZarizeniAktivni.setAddQuotes(true);

    const std::vector<std::string> header{"NAZEV_ZARIZENI",
                                          "DRUH_ZARIZENI",
                                          "OBOR_PECE",
                                          "FORMA_PECE",
                                          "DRUH_PECE",
                                          "ADRESA_ZARIZENI",
                                          "ULICE",
                                          "CISLO_DOMU",
                                          "OBEC",
                                          "PSC",
                                          "KRAJ",
                                          "OKRES",
                                          "NAZEV_POSKYTOVATELE",
                                          "ICO_number",
                                          "ICO",
                                          "ADRESA_POSKYTOVATELE",
                                          "KRAJ_POSKYTOVATELE",
                                          "OKRES_POSKYTOVATELE",
                                          "OBEC_POSKYTOVATELE",
                                          "OBEC_OKRES_POSKYTOVATELE",
                                          "PRAVNI_FORMA_POSKYTOVATELE"};

    auto resultDS = db.execSimpleQuery(queryZarizeni, false, "first_join")->dataSet;
    auto field_NAZEV_ZARIZENI = resultDS->fieldByName("NAZEV_ZARIZENI");
    auto field_DRUH_ZARIZENI = resultDS->fieldByName("DRUH_ZARIZENI");
    auto field_OBOR_PECE = resultDS->fieldByName("OBOR_PECE");
    auto field_FORMA_PECE = resultDS->fieldByName("FORMA_PECE");
    auto field_DRUH_PECE = resultDS->fieldByName("DRUH_PECE");
    // ADRESA_ZARIZENI
    auto field_ULICE = resultDS->fieldByName("ULICE");
    auto field_CISLO_DOMU = resultDS->fieldByName("CISLO_DOMU");
    auto field_OBEC = resultDS->fieldByName("OBEC");
    auto field_PSC = resultDS->fieldByName("PSC");
    auto field_KRAJ = resultDS->fieldByName("KRAJ");
    auto field_OKRES = resultDS->fieldByName("OKRES");
    auto field_NAZEV_POSKYTOVATELE = resultDS->fieldByName("NAZEV_POSKYTOVATELE");
    auto field_ICO_number = dynamic_cast<DataSets::IntegerField*>(resultDS->fieldByName("ICO_number"));
    auto field_ICO = resultDS->fieldByName("ICO");
    auto field_ADRESA_POSKYTOVATELE = resultDS->fieldByName("ADRESA_POSKYTOVATELE");
    auto field_KRAJ_POSKYTOVATELE = resultDS->fieldByName("KRAJ_POSKYTOVATELE");
    auto field_OKRES_POSKYTOVATELE = resultDS->fieldByName("OKRES_POSKYTOVATELE");
    auto field_OBEC_POSKYTOVATELE = resultDS->fieldByName("OBEC_POSKYTOVATELE");
    auto field_OBEC_OKRES_POSKYTOVATELE = resultDS->fieldByName("OBEC_OKRES_POSKYTOVATELE");
    auto field_PRAVNI_FORMA_POSKYTOVATELE = resultDS->fieldByName("PRAVNI_FORMA_POSKYTOVATELE");

    auto aktivniField = resultDS->fieldByName("AKTIVNI");
    writerZarizeni.writeHeader(header);
    writerZarizeniAktivni.writeHeader(header);
    resultDS->resetBegin();
    std::vector<std::string> row{};
    while (resultDS->next()) {
      row.clear();
      row.emplace_back(Utilities::defaultForEmpty(field_NAZEV_ZARIZENI->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_DRUH_ZARIZENI->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OBOR_PECE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_FORMA_PECE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_DRUH_PECE->getAsString(), notAvailable));
      row.emplace_back(
          Utilities::defaultForEmpty(buildAdresa(field_OBEC->getAsString(), field_PSC->getAsString(),
                                                 field_ULICE->getAsString(), field_CISLO_DOMU->getAsString()),
                                     notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ULICE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_CISLO_DOMU->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OBEC->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_PSC->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_KRAJ->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OKRES->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_NAZEV_POSKYTOVATELE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ICO_number->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ICO->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ADRESA_POSKYTOVATELE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_KRAJ_POSKYTOVATELE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OKRES_POSKYTOVATELE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OBEC_POSKYTOVATELE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OBEC_OKRES_POSKYTOVATELE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_PRAVNI_FORMA_POSKYTOVATELE->getAsString(), notAvailable));
      writerZarizeni.writeRecord(row);
      if (aktivniField->getAsString() == "aktivní") {
        writerZarizeniAktivni.writeRecord(row);
      }
    }
  }
  print("DONE");
  return 0;
}