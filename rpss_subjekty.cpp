//
// Created by petr on 3/14/20.
//
#include "LoadingUtils.h"
#include "fmt/format.h"
#include <CsvReader.h>
#include <CsvWriter.h>
#include <MemoryDataBase.h>
#include <io/print.h>
#include <string>
#include <various/isin.h>

using namespace std::string_literals;

const auto csvPath = "/home/petr/Desktop/muni/"s;
const auto outPath = csvPath + "out/";
const auto rpssPath = csvPath + "rpss/Prehled_socialnich_sluzeb_CR_2.3.2020.xlsx";
const auto resPath = csvPath + "res/res_full.csv";
const auto notAvailable = "není k dispozici"s;
const auto justicePath = csvPath + "or.justice/justice.csv";
const auto copniSektorPath = csvPath + "INPUT_pf_copni_okres_sektor.xlsx";
const auto rzpPath = csvPath + "rzp/rzp.csv";
const auto geoPath = csvPath + "geo.csv";
const auto copniPath = csvPath + "CSU2019_copni.xlsx";

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
  const auto currentTime = std::chrono::system_clock::now();
  auto now_t = std::chrono::system_clock::to_time_t(currentTime);
  struct tm *parts = std::localtime(&now_t);
  const auto year = 1900 + parts->tm_year;
  const auto month = 1 + parts->tm_mon;
  const auto downloadPeriodData = fmt::format("{:04d}-{:02d}", year, month);

  DataBase::MemoryDataBase db{"db"};

  std::vector<std::string> okresniMesta;
  {
    DataProviders::CsvReader csvReader("/home/petr/Desktop/muni/res/okresni_mesta.csv");
    std::transform(csvReader.begin(), csvReader.end(), std::back_inserter(okresniMesta),
                   [](const auto &row) { return row[0]; });
  }

  db.addTable(createDataSetFromFile("rzp", FileSettings::Csv(rzpPath),
                                    {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String}));

  db.addTable(shrinkRzp(db.tableByName("rzp")->dataSet));

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

  const std::vector rpssTypes{
      ValueType::String, ValueType::Integer, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::Integer, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String,
  };
  db.addTable(createDataSetFromFile("justice", FileSettings::Csv(justicePath),
                                    {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String}));

  db.addTable(createDataSetFromFile("rpss", FileSettings::Xlsx(rpssPath, "List1"), rpssTypes));
  db.addTable(createDataSetFromFile("res", FileSettings::Csv(resPath, ','),
                                    {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String, ValueType::String, ValueType::String}));

  db.addTable(createDataSetFromFile(
      "prav_formy_SEKTOR", FileSettings::Xlsx(copniSektorPath, "prav_formy_SEKTOR"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String}));

  const auto queryZarizeni = R"(
SELECT
rpss."Název zařízení" as NAZEV_ZARIZENI,
rpss."Druh sociální služby" AS DRUH_SLUZBY,
rpss."Poskytována od" AS DATUM_ZAHAJENI,
rpss."cílová skupina" AS CILOVA_SKUPINA,
rpss."Věková kategorie" AS VEKOVA_KATEGORIE,
rpss."Ambul. forma" AS AMBULANTNI_FORMA,
rpss."Pobyt. forma" AS POBYTOVA_FORMA,
rpss."Teren. forma" AS TERENNI_FORMA,
rpss."Adresa textově - Z" AS ADRESA,
rpss."Kontakty služba - Telefon" AS TELEFON,
rpss."Kontakty služba - Email" AS EMAIL,
rpss."Kontakty služba - Web" AS WEB,
rpss."Kraj - Z" AS KRAJ,
rpss."Okres - Z" AS OKRES,
rpss."Obec - Z" AS OBEC,
rpss."Název" AS NAZEV_POSKYTOVATELE,
res.ICO,
res.ICO_number,
rpss."Adresa textově - P" AS ADRESA_POSKYTOVATELE,
res.KRAJ AS KRAJ_POSKYTOVATELE,
res.OKRES AS OKRES_POSKYTOVATELE,
res.OBEC AS OBEC_POSKYTOVATELE,
res.OBEC_OKRES AS OBEC_OKRES_POSKYTOVATELE,
prav_formy_SEKTOR.sub_sektor,
prav_formy_SEKTOR.sektor,
res.INSTITUCE_V_LIKVIDACI AS AKTIVNI
FROM rpss
LEFT JOIN res ON rpss."IČ" = res.ICO_number
LEFT JOIN prav_formy_SEKTOR ON res.FORMA = prav_formy_SEKTOR.NNO
ORDER BY res.ICO ASC;
)";

  const auto queryPoskytovatelGIS = R"(
SELECT
rpss."Název" AS NAZEV_POSKYTOVATELE,
res.ICO,
res.ICO_number,
justice.UCEL AS POSLANI,
res.VELIKOSTNI_KATEGORIE AS POCET_ZAMESTNANCU,
res.ADRESA,
res.KRAJ,
res.OKRES,
res.OBEC,
res.OBEC_OKRES,
res.PRAVNI_FORMA,
res.DATUM_VZNIKU,
res.ROK_VZNIKU,
prav_formy_SEKTOR.sub_sektor,
prav_formy_SEKTOR.sektor
FROM rpss
LEFT JOIN res on rpss."IČ" = res.ICO_number
LEFT JOIN justice on rpss."IČ" = justice.ICO
LEFT JOIN prav_formy_SEKTOR ON res.FORMA = prav_formy_SEKTOR.NNO
ORDER BY res.ICO ASC;
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
prav_formy_SEKTOR.sub_sektor,
prav_formy_SEKTOR.sektor
FROM rpss
JOIN res ON rpss."IČ" = res.ICO_number
LEFT JOIN justice on rpss."IČ" = justice.ICO
LEFT JOIN copni on res.ICO_number = copni.ICO
left join rzp_shrinked on res.ICO_number = rzp_shrinked.ICO
left join geo on res.ICO_number = geo.ICO
LEFT JOIN prav_formy_SEKTOR ON res.FORMA = prav_formy_SEKTOR.NNO
ORDER BY res.ICO_number ASC;
)";

  std::unordered_map<int, int> zarizeniCountForICO;
  {
    auto zarizeniDS = db.execSimpleQuery(queryZarizeni, false, "rpss_zarizeni")->dataSet;
    auto field_NAZEV_ZARIZENI = zarizeniDS->fieldByName("NAZEV_ZARIZENI");
    auto field_DRUH_SLUZBY = zarizeniDS->fieldByName("DRUH_SLUZBY");
    auto field_DATUM_ZAHAJENI = zarizeniDS->fieldByName("DATUM_ZAHAJENI");
    // rok zahajeni
    auto field_CILOVA_SKUPINA = zarizeniDS->fieldByName("CILOVA_SKUPINA");
    auto field_VEKOVA_KATEGORIE = zarizeniDS->fieldByName("VEKOVA_KATEGORIE");
    auto field_AMBULANTNI_FORMA = zarizeniDS->fieldByName("AMBULANTNI_FORMA");
    auto field_POBYTOVA_FORMA = zarizeniDS->fieldByName("POBYTOVA_FORMA");
    auto field_TERENNI_FORMA = zarizeniDS->fieldByName("TERENNI_FORMA");
    auto field_ADRESA = zarizeniDS->fieldByName("ADRESA");
    auto field_OBEC_OKRES_POSKYTOVATELE = zarizeniDS->fieldByName("OBEC_OKRES_POSKYTOVATELE");
    auto field_TELEFON = zarizeniDS->fieldByName("TELEFON");
    auto field_EMAIL = zarizeniDS->fieldByName("EMAIL");
    auto field_WEB = zarizeniDS->fieldByName("WEB");
    auto field_KRAJ = zarizeniDS->fieldByName("KRAJ");
    auto field_OKRES = zarizeniDS->fieldByName("OKRES");
    auto field_OBEC = zarizeniDS->fieldByName("OBEC");
    auto field_NAZEV_POSKYTOVATELE = zarizeniDS->fieldByName("NAZEV_POSKYTOVATELE");
    auto field_ICO = zarizeniDS->fieldByName("ICO");
    auto field_ICO_number = dynamic_cast<DataSets::IntegerField *>(zarizeniDS->fieldByName("ICO_number"));
    auto field_ADRESA_POSKYTOVATELE = zarizeniDS->fieldByName("ADRESA_POSKYTOVATELE");
    auto field_KRAJ_POSKYTOVATELE = zarizeniDS->fieldByName("KRAJ_POSKYTOVATELE");
    auto field_OKRES_POSKYTOVATELE = zarizeniDS->fieldByName("OKRES_POSKYTOVATELE");
    auto field_OBEC_POSKYTOVATELE = zarizeniDS->fieldByName("OBEC_POSKYTOVATELE");
    auto field_sub_sektor = zarizeniDS->fieldByName("sub_sektor");
    auto field_sektor = zarizeniDS->fieldByName("sektor");
    auto field_AKTIVNI = zarizeniDS->fieldByName("AKTIVNI");
    zarizeniDS->resetBegin();

    DataWriters::CsvWriter rpssZarizeniWriter{outPath + "rpss_zar_gis.csv"};
    DataWriters::CsvWriter rpssZarizeniFullWriter{outPath + "rpss_zar_full.csv"};
    rpssZarizeniWriter.setAddQuotes(true);
    rpssZarizeniFullWriter.setAddQuotes(true);
    const auto header = {
        "NAZEV_ZARIZENI"s,
        "DRUH_SLUZBY"s,
        "DATUM_ZAHAJENI"s,
        "ROK_ZAHAJENI"s,
        "CILOVA_SKUPINA"s,
        "VEKOVA_KATEGORIE"s,
        "AMBULANTNI_FORMA"s,
        "POBYTOVA_FORMA"s,
        "TERENNI_FORMA"s,
        "ADRESA"s,
        "TELEFON"s,
        "EMAIL"s,
        "WEB"s,
        "KRAJ"s,
        "OKRES"s,
        "OBEC"s,
        "OBEC_OKRES"s,
        "NAZEV_POSKYTOVATELE"s,
        "ICO"s,
        "ICO_number"s,
        "ADRESA_POSKYTOVATELE"s,
        "KRAJ_POSKYTOVATELE"s,
        "OKRES_POSKYTOVATELE"s,
        "OBEC_POSKYTOVATELE"s,
        "OBEC_OKRES_POSKYTOVATELE"s,
        "sub_sektor"s,
        "sektor"s,
    };
    rpssZarizeniWriter.writeHeader(header);
    rpssZarizeniFullWriter.writeHeader(header);
    std::vector<std::string> row;
    ExcelDateTime2DateTimeConverter excelDateTime2DateTimeConverter;
    while (zarizeniDS->next()) {
      const auto ico = field_ICO_number->getAsInteger();
      if (!zarizeniCountForICO.contains(ico)) {
        zarizeniCountForICO[ico] = 0;
      }
      ++zarizeniCountForICO[ico];
      row.clear();
      row.emplace_back(Utilities::defaultForEmpty(field_NAZEV_ZARIZENI->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_DRUH_SLUZBY->getAsString(), notAvailable));
      const auto datumZahajeniStr = field_DATUM_ZAHAJENI->getAsString();
      if (datumZahajeniStr.empty()) {
        row.emplace_back(notAvailable);
        row.emplace_back(notAvailable);
      } else {
        auto datumZahajeni = excelDateTime2DateTimeConverter.convert(Utilities::stringToDouble(datumZahajeniStr));
        row.emplace_back(datumZahajeni.toString(DateTime::dateDefFmt));
        row.emplace_back(datumZahajeni.toString("%Y"));
      }
      row.emplace_back(Utilities::defaultForEmpty(field_CILOVA_SKUPINA->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_VEKOVA_KATEGORIE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(Utilities::toUpper(field_AMBULANTNI_FORMA->getAsString()), "NE"));
      row.emplace_back(Utilities::defaultForEmpty(Utilities::toUpper(field_POBYTOVA_FORMA->getAsString()), "NE"));
      row.emplace_back(Utilities::defaultForEmpty(Utilities::toUpper(field_TERENNI_FORMA->getAsString()), "NE"));
      row.emplace_back(Utilities::defaultForEmpty(field_ADRESA->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_TELEFON->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_EMAIL->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_WEB->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_KRAJ->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OKRES->getAsString(), notAvailable));
      const auto obec = field_OBEC->getAsString();
      row.emplace_back(Utilities::defaultForEmpty(obec, notAvailable));
      if (obec.empty()) {
        row.emplace_back(notAvailable);
      } else if (isIn(obec, okresniMesta)) {
        row.emplace_back(obec);
      } else {
        row.emplace_back(fmt::format("{} ({})", obec, field_OKRES->getAsString()));
      }

      row.emplace_back(Utilities::defaultForEmpty(field_NAZEV_POSKYTOVATELE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ICO->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ICO_number->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ADRESA_POSKYTOVATELE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_KRAJ_POSKYTOVATELE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OKRES_POSKYTOVATELE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OBEC_POSKYTOVATELE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_OBEC_OKRES_POSKYTOVATELE->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_sub_sektor->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_sektor->getAsString(), notAvailable));
      if (field_AKTIVNI->getAsString() == "aktivní") {
        rpssZarizeniWriter.writeRecord(row);
      }
      rpssZarizeniFullWriter.writeRecord(row);
    }
  }

  {
    auto poskytovateleDS = db.execSimpleQuery(queryPoskytovatelGIS, false, "poskytovatel_gis")->dataSet;

    DataWriters::CsvWriter rpssPoskytovatelWriter{outPath + "rpss_org_gis.csv"};
    rpssPoskytovatelWriter.setAddQuotes(true);

    auto field_NAZEV_POSKYTOVATELE = poskytovateleDS->fieldByName("NAZEV_POSKYTOVATELE");
    auto field_ICO = poskytovateleDS->fieldByName("ICO");
    auto field_ICO_number = dynamic_cast<DataSets::IntegerField *>(poskytovateleDS->fieldByName("ICO_number"));
    auto field_POSLANI = poskytovateleDS->fieldByName("POSLANI");
    auto field_POCET_ZAMESTNANCU = poskytovateleDS->fieldByName("POCET_ZAMESTNANCU");
    auto field_ADRESA = poskytovateleDS->fieldByName("ADRESA");
    auto field_KRAJ = poskytovateleDS->fieldByName("KRAJ");
    auto field_OKRES = poskytovateleDS->fieldByName("OKRES");
    auto field_OBEC = poskytovateleDS->fieldByName("OBEC");
    auto field_OBEC_OKRES = poskytovateleDS->fieldByName("OBEC_OKRES");
    auto field_PRAVNI_FORMA = poskytovateleDS->fieldByName("PRAVNI_FORMA");
    auto field_DATUM_VZNIKU = poskytovateleDS->fieldByName("DATUM_VZNIKU");
    auto field_ROK_VZNIKU = poskytovateleDS->fieldByName("ROK_VZNIKU");
    auto field_sub_sektor = poskytovateleDS->fieldByName("sub_sektor");
    auto field_sektor = poskytovateleDS->fieldByName("sektor");

    const auto header = {"NAZEV_POSKYTOVATELE"s,
                         "ICO"s,
                         "ICO_number"s,
                         "POSLANI"s,
                         "POCET_ZAMESTNANCU"s,
                         "ADRESA"s,
                         "KRAJ"s,
                         "OKRES"s,
                         "OBEC"s,
                         "OBEC_OKRES"s,
                         "PRAVNI_FORMA"s,
                         "DATUM_VZNIKU"s,
                         "ROK_VZNIKU"s,
                         "sub_sektor"s,
                         "sektor"s,
                         "POCET_ZARIZENI"s};
    rpssPoskytovatelWriter.writeHeader(header);
    poskytovateleDS->resetBegin();
    std::vector<std::string> row;
    int lastICO = 0;
    while (poskytovateleDS->next()) {
      const auto ico = field_ICO_number->getAsInteger();
      if (ico != lastICO) {
        row.clear();
        row.emplace_back(Utilities::defaultForEmpty(field_NAZEV_POSKYTOVATELE->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_ICO->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_ICO_number->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_POSLANI->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_POCET_ZAMESTNANCU->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_ADRESA->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_KRAJ->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_OKRES->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_OBEC->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_OBEC_OKRES->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_PRAVNI_FORMA->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_DATUM_VZNIKU->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_ROK_VZNIKU->getAsString(), notAvailable));
        const auto pocetZarizeni = std::to_string(zarizeniCountForICO[field_ICO_number->getAsInteger()]);
        row.emplace_back(Utilities::defaultForEmpty(field_sub_sektor->getAsString(), notAvailable));
        row.emplace_back(Utilities::defaultForEmpty(field_sektor->getAsString(), notAvailable));
        row.emplace_back(pocetZarizeni);
        rpssPoskytovatelWriter.writeRecord(row);
        lastICO = ico;
      }
    }
  }

  {
    auto poskytovatelRESDS = db.execSimpleQuery(queryPoskytovatelRES, false, "poskytovatel_full")->dataSet;
    auto field_ICO_number = dynamic_cast<DataSets::IntegerField *>(poskytovatelRESDS->fieldByName("ICO_number"));
    auto field_ICO = poskytovatelRESDS->fieldByName("ICO");
    auto field_NAZEV = poskytovatelRESDS->fieldByName("NAZEV");
    auto field_PRAVNI_FORMA = poskytovatelRESDS->fieldByName("PRAVNI_FORMA");
    auto field_VELIKOSTNI_KATEGORIE = poskytovatelRESDS->fieldByName("VELIKOSTNI_KATEGORIE");
    auto field_ADRESA = poskytovatelRESDS->fieldByName("ADRESA");
    auto field_KRAJ = poskytovatelRESDS->fieldByName("KRAJ");
    auto field_OKRES = poskytovatelRESDS->fieldByName("OKRES");
    auto field_OBEC = poskytovatelRESDS->fieldByName("OBEC");
    auto field_OBEC_OKRES = poskytovatelRESDS->fieldByName("OBEC_OKRES");
    auto field_OBEC_ICOB = poskytovatelRESDS->fieldByName("OBEC_ICOB");
    auto field_DATUM_VZNIKU = poskytovatelRESDS->fieldByName("DATUM_VZNIKU");
    auto field_ROK_VZNIKU = poskytovatelRESDS->fieldByName("ROK_VZNIKU");
    auto field_DATUM_LIKVIDACE = poskytovatelRESDS->fieldByName("DATUM_LIKVIDACE");
    auto field_ROK_LIKVIDACE = poskytovatelRESDS->fieldByName("ROK_LIKVIDACE");
    auto field_INSTITUCE_V_LIKVIDACI = poskytovatelRESDS->fieldByName("INSTITUCE_V_LIKVIDACI");
    auto field_COPNI_cinnost = poskytovatelRESDS->fieldByName("COPNI_cinnost");
    auto field_COPNI_obor = poskytovatelRESDS->fieldByName("COPNI_obor");
    auto field_AKTUALNI_INFORMACE_VE_VEREJNEM_REJSTRIKU = poskytovatelRESDS->fieldByName("AKTUALNI_INFORMACE_VE_VEREJNEM_REJSTRIKU");
    auto field_DATUM_ZAPISU = poskytovatelRESDS->fieldByName("DATUM_ZAPISU");
    auto field_NADACNI_KAPITAL = poskytovatelRESDS->fieldByName("NADACNI_KAPITAL");
    auto field_STATUTARNI_ORGAN = poskytovatelRESDS->fieldByName("STATUTARNI_ORGAN");
    auto field_ZAKLADATEL = poskytovatelRESDS->fieldByName("ZAKLADATEL");
    auto field_POSLANI = poskytovatelRESDS->fieldByName("POSLANI");
    auto field_NACE_cinnost = poskytovatelRESDS->fieldByName("NACE_cinnost");
    auto field_NACE_kod = poskytovatelRESDS->fieldByName("NACE_kod");
    auto field_PREDMET_CINNOSTI = poskytovatelRESDS->fieldByName("PREDMET_CINNOSTI");
    auto field_ZIVNOSTENSKE_OPRAVNENI_OBOR = poskytovatelRESDS->fieldByName("ZIVNOSTENSKE_OPRAVNENI_OBOR");
    auto field_ZIVNOSTENSKE_OPRAVNENI = poskytovatelRESDS->fieldByName("ZIVNOSTENSKE_OPRAVNENI");
    auto field_ZIVNOSTENSKE_OPRAVNENI_ODKAZ = poskytovatelRESDS->fieldByName("ZIVNOSTENSKE_OPRAVNENI_ODKAZ");
    auto field_MA_ZIVNOSTENSKE_OPRAVNENI = poskytovatelRESDS->fieldByName("MA_ZIVNOSTENSKE_OPRAVNENI");
    auto field_Y = poskytovatelRESDS->fieldByName("Y");
    auto field_X = poskytovatelRESDS->fieldByName("X");
    auto field_sub_sektor = poskytovatelRESDS->fieldByName("sub_sektor");
    auto field_sektor = poskytovatelRESDS->fieldByName("sektor");

    poskytovatelRESDS->resetBegin();
    const auto header = {
        "ICO_number"s,
        "ICO"s,
        "NAZEV"s,
        "PRAVNI_FORMA"s,
        "VELIKOSTNI_KATEGORIE"s,
        "ADRESA"s,
        "KRAJ"s,
        "OKRES"s,
        "OBEC"s,
        "OBEC_OKRES"s,
        "OBEC_ICOB"s,
        "DATUM_VZNIKU"s,
        "ROK_VZNIKU"s,
        "DATUM_LIKVIDACE"s,
        "ROK_LIKVIDACE"s,
        "INSTITUCE_V_LIKVIDACI"s,
        "COPNI_cinnost"s,
        "COPNI_obor"s,
        "AKTUALNI_INFORMACE_VE_VEREJNEM_REJSTRIKU"s,
        "DATUM_ZAPISU"s,
        "NADACNI_KAPITAL"s,
        "STATUTARNI_ORGAN"s,
        "ZAKLADATEL"s,
        "POCET_ZARIZENI"s,
        "POSLANI"s,
        "NACE_cinnost"s,
        "NACE_kod"s,
        "PREDMET_CINNOSTI"s,
        "ZIVNOSTENSKE_OPRAVNENI_OBOR"s,
        "ZIVNOSTENSKE_OPRAVNENI"s,
        "ZIVNOSTENSKE_OPRAVNENI_ODKAZ"s,
        "MA_ZIVNOSTENSKE_OPRAVNENI"s,
        "Y"s,
        "X"s,
        "download_period"s,
        "sub_sektor"s,
        "sektor"s
    };

    DataWriters::CsvWriter poskytovatelRESWriter{outPath + "rpss_org_full.csv"};
    poskytovatelRESWriter.setAddQuotes(true);
    poskytovatelRESWriter.writeHeader(header);
    std::vector<std::string> row;
    int lastICO = 0;
    while(poskytovatelRESDS->next()) {
      auto ico = field_ICO_number->getAsInteger();
      if (lastICO == ico) {
        continue;
      }
      lastICO = ico;
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
        copniCinnost = "Sociální služby";
      }
      auto copniObor = field_COPNI_obor->getAsString();
      if (copniObor.empty() || copniObor == notAvailable) {
        copniObor = "Sociální služby";
      }
      row.emplace_back(copniCinnost);
      row.emplace_back(copniObor);
      row.emplace_back(Utilities::defaultForEmpty(field_AKTUALNI_INFORMACE_VE_VEREJNEM_REJSTRIKU->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_DATUM_ZAPISU->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_NADACNI_KAPITAL->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_STATUTARNI_ORGAN->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ZAKLADATEL->getAsString(), notAvailable));
      row.emplace_back(std::to_string(zarizeniCountForICO[ico]));
      row.emplace_back(Utilities::defaultForEmpty(field_POSLANI->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_NACE_cinnost->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_NACE_kod->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_PREDMET_CINNOSTI->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ZIVNOSTENSKE_OPRAVNENI_OBOR->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ZIVNOSTENSKE_OPRAVNENI->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_ZIVNOSTENSKE_OPRAVNENI_ODKAZ->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_MA_ZIVNOSTENSKE_OPRAVNENI->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_Y->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_X->getAsString(), notAvailable));
      row.emplace_back(downloadPeriodData);
      row.emplace_back(Utilities::defaultForEmpty(field_sub_sektor->getAsString(), notAvailable));
      row.emplace_back(Utilities::defaultForEmpty(field_sektor->getAsString(), notAvailable));
      poskytovatelRESWriter.writeRecord(row);
    }
  }
}