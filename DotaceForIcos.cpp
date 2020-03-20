//
// Created by petr on 3/19/20.
//
#include "DotaceForIcos.h"

#include "Combiner.h"
#include "LoadingUtils.h"
#include "fmt/format.h"
#include "time/now.h"
#include <CsvReader.h>
#include <CsvStreamReader.h>
#include <CsvWriter.h>
#include <MemoryDataBase.h>
#include <MemoryDataSet.h>
#include <XlntReader.h>
#include <XlntWriter.h>
#include <XlsxIOReader.h>
#include <XlsxReader.h>
#include <io/logger.h>
#include <memory>
#include <various/isin.h>

using namespace std::string_literals;

const auto notAvailable = "není k dispozici"s;
Logger logger{std::cout};

const auto verejneSbirkyPath = "/home/petr/Desktop/muni/verejnesbirky/"s;
const auto muniPath = "/home/petr/Desktop/muni/"s;
const auto dotacePath = "/home/petr/Desktop/muni/dotaceeu/"s;
const auto verejneZakazkyPath = "/home/petr/Desktop/muni/verejnezakazky/"s;
const auto cedrPath = "/home/petr/Desktop/muni/cedr/out/"s;

struct TableColumn {
  std::string table;
  std::string column;
};

struct DSFieldCnt {
  std::shared_ptr<DataSets::BaseDataSet> ds;
  DataSets::IntegerField *field;
  gsl::index pos;
  gsl::index fieldOffset;
  std::vector<DataSets::BaseField *> fields;
};
std::shared_ptr<DataSets::MemoryDataSet> combine(std::vector<std::shared_ptr<DataSets::BaseDataSet>> dataSets,
                                                 const std::vector<TableColumn> &joins) {
  auto result = std::make_shared<DataSets::MemoryDataSet>("combined");
  std::vector<DSFieldCnt> dss;
  const auto recordTypeInfoColumnCount = 1;
  gsl::index fieldOffset = 0;
  bool first = true;
  for (auto [name, column] : joins) {
    auto ds = *std::find_if(dataSets.begin(), dataSets.end(), [name](auto &ds) { return ds->getName() == name; });
    auto field = dynamic_cast<DataSets::IntegerField *>(ds->fieldByName(column));
    DataSets::SortOptions options;
    options.addOption(field, SortOrder::Ascending);
    ds->sort(options);

    ds->resetEnd();
    logger.log<LogLevel::Debug, true>(ds->getName(), ", count: ", ds->getCurrentRecord());

    ds->resetBegin();

    dss.emplace_back(DSFieldCnt{ds, field, 0, fieldOffset, ds->getFields()});
    fieldOffset += ds->getFieldNames().size();
    if (first) {
      fieldOffset += recordTypeInfoColumnCount;
      first = false;
    }
  }
  int rowLength = fieldOffset;
  auto mainDS = dss[0];
  auto mainField = dynamic_cast<DataSets::IntegerField *>(dss[0].field);

  std::vector<std::string> columnNames;
  std::vector<ValueType> columnValueTypes;

  std::transform(mainDS.fields.begin(), mainDS.fields.end(), std::back_inserter(columnNames),
                 [](auto &field) { return field->getName().data(); });
  columnNames.emplace_back("Zdroj dat");
  dss = std::vector<DSFieldCnt>{dss.begin() + 1, dss.end()};
  for (auto &ds : dss) {
    std::transform(ds.fields.begin(), ds.fields.end(), std::back_inserter(columnNames),
                   [](auto &field) { return field->getName().data(); });
  }
  columnNames.emplace_back("download_period");

  std::transform(mainDS.fields.begin(), mainDS.fields.end(), std::back_inserter(columnValueTypes),
                 [](auto &field) { return field->getFieldType(); });
  columnValueTypes.emplace_back(ValueType::String);
  for (auto &ds : dss) {
    std::transform(ds.fields.begin(), ds.fields.end(), std::back_inserter(columnValueTypes),
                   [](auto &field) { return field->getFieldType(); });
  }
  columnValueTypes.emplace_back(ValueType::String);

  result->openEmpty(columnNames, columnValueTypes);
  auto cnt = 0;

  const auto currentTime = std::chrono::system_clock::now();
  auto now_t = std::chrono::system_clock::to_time_t(currentTime);
  struct tm *parts = std::localtime(&now_t);
  const auto year = 1900 + parts->tm_year;
  const auto month = 1 + parts->tm_mon;
  const auto downloadPeriodData = fmt::format("{:04d}-{:02d}", year, month);
  auto dotaceRokField = dataSets[2]->fieldByName("DOTACE_ROK");

  std::vector<std::string> row;
  while (mainDS.ds->next()) {

    if (dotaceRokField->getAsString() == notAvailable) {
      dotaceRokField->setAsString("9999");
    }

    ++cnt;
    for (auto &ds : dss) {
      ds.ds->setCurrentRecord(ds.pos);
      while (ds.ds->next()) {
        if (mainField->getAsInteger() == ds.field->getAsInteger()) {
          row.clear();
          std::transform(mainDS.fields.begin(), mainDS.fields.end(), std::back_inserter(row),
                         [](auto &field) { return Utilities::defaultForEmpty(field->getAsString(), notAvailable); });

          if (ds.ds->getName() == "verSb") {
            row.emplace_back("VerejnaSbirka");
          } else if (ds.ds->getName() == "vz2") {
            row.emplace_back("VerejneZakazky");
          } else if (ds.ds->getName() == "cedrDotace") {
            if (ds.ds->fieldByIndex(4)->getAsString() == notAvailable) {
              row.emplace_back("CEDR");
            } else {
              row.emplace_back("DotaceEU");
            }
          }
          int fill = ds.fieldOffset - mainDS.fields.size() - recordTypeInfoColumnCount;
          for (int i = 0; i < fill; ++i) {
            row.emplace_back(notAvailable);
          }
          std::transform(ds.fields.begin(), ds.fields.end(), std::back_inserter(row),
                         [](auto &field) { return Utilities::defaultForEmpty(field->getAsString(), notAvailable); });

          fill = rowLength - row.size();
          for (int i = 0; i < fill; ++i) {
            row.emplace_back(notAvailable);
          }
          row.emplace_back(downloadPeriodData);

          result->append();
          for (auto i : MakeRange::range(row.size())) {
            result->fieldByIndex(i)->setAsString(row[i]);
          }
        } else if (mainField->getAsInteger() < ds.field->getAsInteger()) {
          ds.pos = ds.ds->getCurrentRecord() - 1;
          break;
        }
      }
    }
  }
  return result;
}

std::shared_ptr<DataSets::MemoryDataSet> transformDotace(const std::shared_ptr<DataSets::BaseDataSet> &dotaceSub) {
  auto dotaceRequired = std::make_shared<DataSets::MemoryDataSet>("dotace2");
  dotaceRequired->openEmpty(
      {"DOTACE_POSKYTOVATEL",
       "DOTACE_CASTKA_UVOLNENA",
       "DOTACE_CASTKA_CERPANA",
       "DOTACE_ROK",
       "ROK_UDELENI_DOTACE",
       "DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU",
       "DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA",
       "DOTACE_REGISTRACNI_CISLO_PROJEKTU",
       "DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU",
       "DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU",
       "DOTACE_OPERACNI_PROGRAM_PRIJEMCE",
       "DOTACE_OPERACNI_PROGRAM_DATUM_SKUTECNE_DATUM_ZAHAJENI_FYZICKE_REALIZACE_PROJEKTU",
       "DOTACE_OPERACNI_PROGRAM_DATUM_REALIZACE_SKUTECNE_DATUM_UKONCENI_FYZICKE_REALIZACE_PROJEKTU",
       "DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_KOD",
       "DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_NAZEV",
       "DOTACE_OPERACNI_PROGRAM_MISTO_REALIZACE_NUTS3_NAZEV",
       "DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_CASTKA",
       "DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_EU",
       "DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR",
       "DOTACE_FINANCNI_ZDROJ_KOD",
       "DOTACE_FINANCNI_ZDROJ_NAZEV"},
      {ValueType::String,  ValueType::Currency, ValueType::Currency, ValueType::String,   ValueType::String,
       ValueType::String,  ValueType::String,   ValueType::String,   ValueType::String,   ValueType::String,
       ValueType::Integer, ValueType::String,   ValueType::String,   ValueType::String,   ValueType::String,
       ValueType::String,  ValueType::Currency, ValueType::Currency, ValueType::Currency, ValueType::String,
       ValueType::String});

  dotaceSub->resetBegin();
  auto DOTACE_POSKYTOVATEL = dotaceSub->fieldByName("Poskytovatel");
  auto DOTACE_CASTKA_UVOLNENA1 = dynamic_cast<DataSets::CurrencyField *>(
      dotaceSub->fieldByName("Celkové způsobilé výdaje přidělené na operaci (příspěvek Unie) CZK"));
  auto DOTACE_CASTKA_UVOLNENA2 = dynamic_cast<DataSets::CurrencyField *>(dotaceSub->fieldByName(
      "Celkové způsobilé výdaje přidělené na operaci (národní soukromé zdroje / soukromé zdroje) CZK"));

  auto DOTACE_CASTKA_CERPANA1 = dynamic_cast<DataSets::CurrencyField *>(
      dotaceSub->fieldByName("Finanční prostředky vyúčtované v žádostech o platbu (příspěvek Unie, CZK)"));
  auto DOTACE_CASTKA_CERPANA2 = dynamic_cast<DataSets::CurrencyField *>(
      dotaceSub->fieldByName("Finanční prostředky vyúčtované v žádostech o platbu (národní veřejné zdroje, CZK)"));

  // auto DOTACE_ROK = dotaceSub->fieldByName("Datum zahájení fyzické operace");
  auto DOTACE_ROK_UDELENI = dotaceSub->fieldByName("Datum zahájení fyzické operace");

  auto DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU = dotaceSub->fieldByName("Program");
  auto DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA = dotaceSub->fieldByName("Název prioritní osy / priority Unie");
  auto DOTACE_REGISTRACNI_CISLO_PROJEKTU = dotaceSub->fieldByName("Registrační číslo projektu/operace");
  auto DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU = dotaceSub->fieldByName("Název projektu / Název operace");
  auto DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU = dotaceSub->fieldByName("Shrnutí operace");
  auto DOTACE_OPERACNI_PROGRAM_PRIJEMCE = dotaceSub->fieldByName("IC");
  auto DOTACE_OPERACNI_PROGRAM_DATUM__SKUTECNE_DATUM_ZAHAJENI_FYZICKE_REALIZACE_PROJEKTU =
      dotaceSub->fieldByName("Skutečné datum ukončení fyzické realizace operace");
  auto DOTACE_OPERACNI_PROGRAM_DATUM_REALIZACE_SKUTECNE_DATUM_UKONCENI_FYZICKE_REALIZACE_PROJEKTU =
      dotaceSub->fieldByName("Skutečné datum ukončení fyzické realizace operace");
  auto DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_KOD = dotaceSub->fieldByName("Oblast intervence - kód");
  auto DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_NAZEV = dotaceSub->fieldByName("Oblast intervence - název");
  auto DOTACE_OPERACNI_PROGRAM_MISTO_REALIZACE_NUTS3_NAZEV = dotaceSub->fieldByName("Název NUTS 3");
  auto DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_CASTKA = dynamic_cast<DataSets::CurrencyField *>(dotaceSub->fieldByName(
      "Celkové způsobilé výdaje přidělené na operaci (národní soukromé zdroje / soukromé zdroje) CZK"));
  auto DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_EU = dynamic_cast<DataSets::CurrencyField *>(
      dotaceSub->fieldByName("Celkové způsobilé výdaje přidělené na operaci (příspěvek Unie) CZK"));
  auto DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR = dynamic_cast<DataSets::CurrencyField *>(
      dotaceSub->fieldByName("Celkové způsobilé výdaje přidělené na operaci (veřejné zdroje ČR) CZK"));
  while (dotaceSub->next()) {
    dotaceRequired->append();
    dotaceRequired->fieldByIndex(0)->setAsString(DOTACE_POSKYTOVATEL->getAsString());
    reinterpret_cast<DataSets::CurrencyField *>(dotaceRequired->fieldByIndex(1))
        ->setAsCurrency(DOTACE_CASTKA_UVOLNENA1->getAsCurrency() + DOTACE_CASTKA_UVOLNENA2->getAsCurrency());
    reinterpret_cast<DataSets::CurrencyField *>(dotaceRequired->fieldByIndex(2))
        ->setAsCurrency(DOTACE_CASTKA_CERPANA1->getAsCurrency() + DOTACE_CASTKA_CERPANA2->getAsCurrency());

    auto rok = DOTACE_ROK_UDELENI->getAsString();
    if (rok.size() > 4) {
      rok = std::string(rok.end() - 4, rok.end());
    }
    dotaceRequired->fieldByIndex(3)->setAsString("2014-2020");
    dotaceRequired->fieldByIndex(4)->setAsString(rok);
    dotaceRequired->fieldByIndex(5)->setAsString(DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU->getAsString());
    dotaceRequired->fieldByIndex(6)->setAsString(DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA->getAsString());
    dotaceRequired->fieldByIndex(7)->setAsString(DOTACE_REGISTRACNI_CISLO_PROJEKTU->getAsString());
    dotaceRequired->fieldByIndex(8)->setAsString(DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU->getAsString());
    dotaceRequired->fieldByIndex(9)->setAsString(DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU->getAsString());
    dotaceRequired->fieldByIndex(10)->setAsString(DOTACE_OPERACNI_PROGRAM_PRIJEMCE->getAsString());
    dotaceRequired->fieldByIndex(11)->setAsString(
        DOTACE_OPERACNI_PROGRAM_DATUM__SKUTECNE_DATUM_ZAHAJENI_FYZICKE_REALIZACE_PROJEKTU->getAsString());
    dotaceRequired->fieldByIndex(12)->setAsString(
        DOTACE_OPERACNI_PROGRAM_DATUM_REALIZACE_SKUTECNE_DATUM_UKONCENI_FYZICKE_REALIZACE_PROJEKTU->getAsString());
    dotaceRequired->fieldByIndex(13)->setAsString(DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_KOD->getAsString());
    dotaceRequired->fieldByIndex(14)->setAsString(DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_NAZEV->getAsString());
    dotaceRequired->fieldByIndex(15)->setAsString(DOTACE_OPERACNI_PROGRAM_MISTO_REALIZACE_NUTS3_NAZEV->getAsString());
    reinterpret_cast<DataSets::CurrencyField *>(dotaceRequired->fieldByIndex(16))
        ->setAsCurrency(DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_CASTKA->getAsCurrency());
    reinterpret_cast<DataSets::CurrencyField *>(dotaceRequired->fieldByIndex(17))
        ->setAsCurrency(DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_EU->getAsCurrency());
    reinterpret_cast<DataSets::CurrencyField *>(dotaceRequired->fieldByIndex(18))
        ->setAsCurrency(DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR->getAsCurrency());
    dotaceRequired->fieldByIndex(19)->setAsString("není k dispozici");
    dotaceRequired->fieldByIndex(20)->setAsString("není k dispozici");
  }
  return dotaceRequired;
}

std::string removeQuotes(std::string_view str) {
  int beginOffset = str[0] == '"' ? 1 : 0;
  int endOffset = str[str.size() - 1] == '"' ? 1 : 0;
  return std::string(str.begin() + beginOffset, str.end() - endOffset);
}

std::shared_ptr<DataSets::MemoryDataSet> transformCedr(const std::shared_ptr<DataSets::BaseDataSet> &cedr) {
  auto fieldNames = cedr->getFieldNames();
  fieldNames.insert(fieldNames.begin() + 4, "DOTACE_ROK_UDELENI");
  std::vector<ValueType> fieldTypes;
  for (auto field : cedr->getFields()) {
    fieldTypes.emplace_back(field->getFieldType());
  }
  fieldTypes.insert(fieldTypes.begin() + 4, ValueType::String);
  fieldNames.insert(fieldNames.begin() + 9, "DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR");
  fieldTypes.insert(fieldTypes.begin() + 9, ValueType::String);
  auto result = std::make_shared<DataSets::MemoryDataSet>("cedrReady");
  result->openEmpty(fieldNames, fieldTypes);

  cedr->resetBegin();
  auto cedrFields = cedr->getFields();
  auto resultFields = result->getFields();
  auto duplField = cedr->fieldByName("DOTACE_CASTKA_UVOLNENA");
  while (cedr->next()) {
    result->append();
    int cnt = 0;
    for (auto field : cedrFields) {
      if (cnt == 4) {
        resultFields[cnt]->setAsString("není k dispozici");
        ++cnt;
      } else if (cnt == 9) {
        resultFields[cnt]->setAsString(duplField->getAsString());
        ++cnt;
      }
      resultFields[cnt]->setAsString(removeQuotes(field->getAsString()));
      ++cnt;
    }
  }
  result->fieldByName("ICOnum")->setName("DOTACE_OPERACNI_PROGRAM_PRIJEMCE");
  return result;
}

std::shared_ptr<DataSets::MemoryDataSet> appendCedrDotace(const std::shared_ptr<DataSets::BaseDataSet> &cedr,
                                                          const std::shared_ptr<DataSets::BaseDataSet> &dotace) {
  cedr->resetBegin();
  dotace->resetBegin();
  auto fieldNames = dotace->getFieldNames();
  std::vector<ValueType> fieldTypes;
  for (auto field : dotace->getFields()) {
    fieldTypes.emplace_back(field->getFieldType());
  }

  auto result = std::make_shared<DataSets::MemoryDataSet>("cedrDotace");
  result->openEmpty(fieldNames, fieldTypes);
  auto resultFields = result->getFields();

  auto dotaceFields = dotace->getFields();
  dotace->resetBegin();
  while (dotace->next()) {
    result->append();
    int cnt = 0;
    for (auto field : dotaceFields) {
      resultFields[cnt]->setAsString(field->getAsString());
      ++cnt;
    }
  }

  auto cedrFields = cedr->getFields();
  cedr->resetBegin();

  auto prijemceField = result->fieldByName("DOTACE_OPERACNI_PROGRAM_PRIJEMCE");
  auto prispevekField = result->fieldByName("DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR");
  auto finKod = result->fieldByName("DOTACE_FINANCNI_ZDROJ_KOD");
  auto finNazev = result->fieldByName("DOTACE_FINANCNI_ZDROJ_NAZEV");
  while (cedr->next()) {
    result->append();
    for (int i = 0; i < static_cast<int>(cedrFields.size()) - 4; ++i) {
      resultFields[i]->setAsString(cedrFields[i]->getAsString());
    }
    for (int i = cedrFields.size() - 4; i < static_cast<int>(resultFields.size() - 2); ++i) {
      resultFields[i]->setAsString(notAvailable);
    }
    prijemceField->setAsString(cedrFields[10]->getAsString());
    prispevekField->setAsString(cedrFields[9]->getAsString());
    finKod->setAsString(cedr->fieldByIndex(11)->getAsString());
    finNazev->setAsString(cedr->fieldByIndex(12)->getAsString());
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
const auto csvPath = "/home/petr/Desktop/muni/"s;
const auto copniPath = csvPath + "CSU2019_copni.xlsx";
const auto geoPath = csvPath + "geo.csv";

std::shared_ptr<DataSets::MemoryDataSet> exportForRes(const std::shared_ptr<DataSets::MemoryDataSet> &res) {
  logger.startTime();
  DataBase::MemoryDataBase db("db");

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
  db.addTable(createDataSetFromFile("geo", FileSettings::Csv(geoPath),
                                    {ValueType::Integer, ValueType::String, ValueType::String}));

  const auto copniQuery =
      R"(select copni1.ICO, copni2."Popis cinnosti", copni2."Oblast cinnosti"
from copni1
join copni2 on copni1.COPNI = copni2.COPNI_codes;)";
  db.addTable(db.execSimpleQuery(copniQuery, false, "copni")->dataSet->toDataSet());
  db.removeTable("copni1");
  db.removeTable("copni2");

  db.addTable(res);
  // db.addTable(createDataSetFromFile("res", FileSettings::Csv(muniPath + "res/res.csv", ','),
  //                                  {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
  //                                   ValueType::String, ValueType::String, ValueType::String, ValueType::String,
  //                                   ValueType::String, ValueType::String, ValueType::String, ValueType::String,
  //                                   ValueType::String, ValueType::String, ValueType::String, ValueType::String,
  //                                   ValueType::String, ValueType::String, ValueType::String}));

  const auto resQuery = R"(
SELECT res.ICO_number, res.ICO, res.NAZEV, res.PRAVNI_FORMA, res.VELIKOSTNI_KATEGORIE, res.ADRESA, res.KRAJ,
res.OKRES, res.OBEC, res.OBEC_OKRES, res.OBEC_ICOB, res.DATUM_VZNIKU, res.ROK_VZNIKU, res.DATUM_LIKVIDACE,
res.ROK_LIKVIDACE, res.INSTITUCE_V_LIKVIDACI, copni."Popis cinnosti" as COPNI_cinnost,
copni."Oblast cinnosti" AS COPNI_obor, geo.x as Y, geo.y as X
FROM res
left join copni on res.ICO_number = copni.ICO
left join geo on res.ICO_number = geo.ICO;
)";
  auto neziskovyView = db.execSimpleQuery(resQuery, false, "res");

  db.addTable(createDataSetFromFile(
      "verejneSbirky", FileSettings::Xlsx(verejneSbirkyPath + "CESSeznamIM08_cor2020-03-09.xlsx", "uprava_dat"),
      {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String}));

  db.execSimpleQuery("select verejneSbirky.NAZEV_PO, verejneSbirky.OZNACENI_SBIRKY AS VEREJNA_SBIRKA_OZNACENI, "
                     "verejneSbirky.UZEMNI_ROZSAH AS "
                     "VEREJNA_SBIRKA_UZEMNI_ROZSAH, verejneSbirky.DATUM_ZAHAJENI AS VEREJNA_SBIRKA_ZACATEK, "
                     "verejneSbirky.DATUM_UKONCENI AS VEREJNA_SBIRKA_KONEC, verejneSbirky.ZPUSOB_PROVADENI AS "
                     "VEREJNA_SBIRKA_ZPUSOB, verejneSbirky.UCEL_SBIRKY AS VEREJNA_SBIRKA_UCEL, "
                     "verejneSbirky.UCEL_TEXTEM AS VEREJNA_SBIRKA_UCEL_TEXTEM, res.ICO_number "
                     "from verejneSbirky "
                     "JOIN res on verejneSbirky.NAZEV_PO = res.NAZEV;",
                     true, "verSb");

  auto VZds = createDataSetFromFile(
      "VZ", FileSettings::Xlsx(verejneZakazkyPath + "vz.xlsx", "VZ"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String});

  auto castiVZds = createDataSetFromFile(
      "castiVZ", FileSettings::Xlsx(verejneZakazkyPath + "vz.xlsx", "Části VZ"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String});
  auto dodavatelVZ = createDataSetFromFile(
      "dodVZ", FileSettings::Xlsx(verejneZakazkyPath + "vz.xlsx", "Dodavatelé"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String});

  db.addTable(VZds);
  db.addTable(castiVZds);
  db.addTable(dodavatelVZ);

  const std::string query =
      "select VZ.EvidencniCisloVZnaVVZ AS VEREJNA_ZAKAZKA_ID_ZAKAZKY, "
      "VZ.LimitVZ AS VEREJNA_ZAKAZKA_LIMIT_VZ, "
      "VZ.CelkovaKonecnaHodnotaVZ AS VEREJNA_ZAKAZKA_CELKOVA_KONECNA_HODNOTA, "
      "VZ.ZadavatelICO AS VEREJNA_ZAKAZKA_ZADAVATEL_IC, "
      "VZ.DatumUverejneni AS VEREJNA_ZAKAZKA_DATUM_ZADANI, "
      "VZ.NazevVZ AS VEREJNA_ZAKAZKA_NAZEV_VZ, "
      "VZ.DruhVZ AS VEREJNA_ZAKAZKA_DRUH_VZ, "
      "VZ.StrucnyPopisVZ AS VEREJNA_ZAKAZKA_STRUCNY_POPIS_VZ, "
      "VZ.ZadavatelUredniNazev AS VEREJNA_ZAKAZKA_ZADAVATEL_UREDNI_NAZEV, "
      "castiVZ.ID_CastiVZ AS VEREJNA_ZAKAZKA_CISLO_CASTI_ZADANI_VZ, "
      "castiVZ.NazevCastiVZ AS VEREJNA_ZAKAZKA_NAZEV_CASTI_VZ, "
      "castiVZ.PredpokladanaCelkovaHodnotaCastiVZ as VEREJNA_ZAKAZKA_PREPOKLADANA_CELKOVA_HODNOTA_CASTI_VZ, "
      "dodVZ.DodavatelICO as DODAVATEL_ICO from VZ "
      "join castiVZ on VZ.ID_Zakazky = castiVZ.ID_Zakazky "
      "join dodVZ on VZ.ID_Zakazky = dodVZ.ID_Zakazky "
      "join res on dodVZ.DodavatelICO = res.ICO_number;";

  db.addTable(db.execSimpleQuery(query, false, "vz")->dataSet->toDataSet());
  logger.log<LogLevel::Info>("Query vz done");
  db.tableByName("vz")->dataSet->resetEnd();
  logger.log<LogLevel::Debug, true>(db.tableByName("vz")->dataSet->getCurrentRecord());

  auto cedr = createDataSetFromFile(
      "cedr", FileSettings::Xlsx(cedrPath + "cedr_output.xlsx"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::Integer,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String});
  db.addTable(cedr);

  auto cedrView = db.execSimpleQuery("select "
                                     "cedr.dotacePoskytovatelNazev AS DOTACE_POSKYTOVATEL, "
                                     "cedr.castkaUvolnena AS DOTACE_CASTKA_UVOLNENA, "
                                     "cedr.castkaCerpana AS DOTACE_CASTKA_CERPANA, "
                                     "cedr.rozpoctoveObdobi AS DOTACE_ROK, "
                                     "cedr.operacniProgramNazev AS DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU, "
                                     "cedr.dotaceTitulNazev AS DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA, "
                                     "cedr.idRozhodnuti AS DOTACE_REGISTRACNI_CISLO_PROJEKTU, "
                                     "cedr.projektNazev AS DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU, "
                                     "cedr.ICOnum, cedr.financniZdrojKod AS DOTACE_FINANCNI_ZDROJ_KOD, "
                                     "cedr.financniZdrojNazev AS DOTACE_FINANCNI_ZDROJ_NAZEV from cedr;",
                                     false, "cedrView");
  auto cedrReady = transformCedr(cedrView->dataSet);

  logger.log<LogLevel::Info>("Query cedr done");

  auto dotaceDS = createDataSetFromFile(
      "dotace", FileSettings::Csv(dotacePath + "dotaceeu.csv"),
      {ValueType::String,   ValueType::String,   ValueType::String,   ValueType::String,   ValueType::String,
       ValueType::String,   ValueType::Integer,  ValueType::String,   ValueType::String,   ValueType::String,
       ValueType::String,   ValueType::String,   ValueType::String,   ValueType::String,   ValueType::String,
       ValueType::String,   ValueType::String,   ValueType::String,   ValueType::String,   ValueType::String,
       ValueType::Currency, ValueType::Currency, ValueType::Currency, ValueType::Currency, ValueType::Currency,
       ValueType::Currency, ValueType::Currency, ValueType::Currency});
  db.addTable(dotaceDS);

  auto dotaceDoplneniDS =
      createDataSetFromFile("dotace_dopl", FileSettings::Xlsx(muniPath + "NNO_doplneni.xlsx", "dotaceeu_poskytovatel"),
                            {ValueType::String, ValueType::String});
  db.addTable(dotaceDoplneniDS);
  auto dotaceSub = db.execSimpleQuery("select dotace.*, "
                                      "res.ICO_number, dotace_dopl.Poskytovatel from dotace "
                                      "join res on dotace.IC = res.ICO_number "
                                      "left join dotace_dopl on dotace.Program = dotace_dopl.Program;",
                                      false, "dotaceSub");

  dotaceSub->dataSet->resetEnd();
  logger.log<LogLevel::Info>("Query dotace2done: ", dotaceSub->dataSet->getCurrentRecord());
  dotaceSub->dataSet->resetBegin();

  auto dotace2 = transformDotace(dotaceSub->dataSet);

  db.execSimpleQuery("select vz.* from vz join res on vz.DODAVATEL_ICO = res.ICO_number;", true, "vz2");
  logger.log<LogLevel::Info>("Query vz2 done");

  auto wtf = appendCedrDotace(cedrReady, dotace2);
  db.addTable(wtf);
  auto cdtc = db.execSimpleQuery("select cedrDotace.* from cedrDotace;", true, "cedrDotace");
  cdtc->dataSet->resetEnd();
  logger.log<LogLevel::Info>("Query cedrEU: ", cdtc->dataSet->getCurrentRecord());
  cdtc->dataSet->resetBegin();

  db.addTable(
      combine({neziskovyView->dataSet, db.viewByName("vz2")->dataSet, cdtc->dataSet, db.viewByName("verSb")->dataSet},
              {{"res", "ICO_number"},
               {"vz2", "DODAVATEL_ICO"},
               {"cedrDotace", "DOTACE_OPERACNI_PROGRAM_PRIJEMCE"},
               {"verSb", "O_ICO"}}));

  db.removeTable("dotace_dopl");
  db.removeTable("dotace");
  db.removeTable("cedr");
  db.removeTable("dodVZ");
  db.removeTable("castiVZ");
  db.removeTable("VZ");
  db.removeTable("verejneSbirky");
  db.removeTable("res");
  db.removeTable("copni");

  const auto queryColumnReorder = R"(
SELECT
combined.ICO_number, combined.ICO, combined.NAZEV, combined.PRAVNI_FORMA, combined.VELIKOSTNI_KATEGORIE,
combined.ADRESA, combined.KRAJ, combined.OKRES, combined.OBEC, combined.OBEC_OKRES, combined.OBEC_ICOB,
combined.DATUM_VZNIKU, combined.ROK_VZNIKU, combined.DATUM_LIKVIDACE, combined.ROK_LIKVIDACE,
combined.INSTITUCE_V_LIKVIDACI, combined.COPNI_cinnost, combined.COPNI_obor, combined."Zdroj dat",
combined.VEREJNA_ZAKAZKA_ID_ZAKAZKY,
combined.VEREJNA_ZAKAZKA_LIMIT_VZ,
combined.VEREJNA_ZAKAZKA_CELKOVA_KONECNA_HODNOTA,
combined.VEREJNA_ZAKAZKA_ZADAVATEL_IC,
combined.VEREJNA_ZAKAZKA_DATUM_ZADANI,
combined.VEREJNA_ZAKAZKA_NAZEV_VZ,
combined.VEREJNA_ZAKAZKA_DRUH_VZ,
combined.VEREJNA_ZAKAZKA_STRUCNY_POPIS_VZ,
combined.VEREJNA_ZAKAZKA_ZADAVATEL_UREDNI_NAZEV,
combined.VEREJNA_ZAKAZKA_CISLO_CASTI_ZADANI_VZ,
combined.VEREJNA_ZAKAZKA_NAZEV_CASTI_VZ,
combined.VEREJNA_ZAKAZKA_PREPOKLADANA_CELKOVA_HODNOTA_CASTI_VZ,
combined.DODAVATEL_ICO,
combined.DOTACE_POSKYTOVATEL,
combined.DOTACE_CASTKA_UVOLNENA,
combined.DOTACE_CASTKA_CERPANA,
combined.DOTACE_ROK AS OBDOBI,
combined.ROK_UDELENI_DOTACE AS PROGRAMOVE_OBDOBI,
combined.DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU,
combined.DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA,
combined.DOTACE_REGISTRACNI_CISLO_PROJEKTU,
combined.DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU,
combined.DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU,
combined.DOTACE_OPERACNI_PROGRAM_PRIJEMCE,
combined.DOTACE_OPERACNI_PROGRAM_DATUM_SKUTECNE_DATUM_ZAHAJENI_FYZICKE_REALIZACE_PROJEKTU,
combined.DOTACE_OPERACNI_PROGRAM_DATUM_REALIZACE_SKUTECNE_DATUM_UKONCENI_FYZICKE_REALIZACE_PROJEKTU,
combined.DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_KOD,
combined.DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_NAZEV,
combined.DOTACE_OPERACNI_PROGRAM_MISTO_REALIZACE_NUTS3_NAZEV,
combined.DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_CASTKA,
combined.DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_EU,
combined.DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR,
combined.DOTACE_FINANCNI_ZDROJ_KOD,
combined.DOTACE_FINANCNI_ZDROJ_NAZEV,
combined.NAZEV_PO,
combined.VEREJNA_SBIRKA_OZNACENI,
combined.VEREJNA_SBIRKA_UZEMNI_ROZSAH,
combined.VEREJNA_SBIRKA_ZACATEK,
combined.VEREJNA_SBIRKA_KONEC,
combined.VEREJNA_SBIRKA_ZPUSOB,
combined.VEREJNA_SBIRKA_UCEL,
combined.VEREJNA_SBIRKA_UCEL_TEXTEM,
combined.download_period,
combined.X,
combined.Y
FROM combined;)";

  //DataWriters::CsvWriter writer{csvPath + "test.csv"};
  //writer.setAddQuotes(true);
  auto finDs = db.execSimpleQuery(queryColumnReorder, false, "reordered")->dataSet;

  auto field_ICO_number = dynamic_cast<DataSets::IntegerField*>(finDs->fieldByName("ICO_number"));
  auto field_ICO = finDs->fieldByName("ICO");
  auto field_NAZEV = finDs->fieldByName("NAZEV");
  auto field_PRAVNI_FORMA = finDs->fieldByName("PRAVNI_FORMA");
  auto field_VELIKOSTNI_KATEGORIE = finDs->fieldByName("VELIKOSTNI_KATEGORIE");
  auto field_ADRESA = finDs->fieldByName("ADRESA");
  auto field_KRAJ = finDs->fieldByName("KRAJ");
  auto field_OKRES = finDs->fieldByName("OKRES");
  auto field_OBEC = finDs->fieldByName("OBEC");
  auto field_OBEC_OKRES = finDs->fieldByName("OBEC_OKRES");
  auto field_OBEC_ICOB = finDs->fieldByName("OBEC_ICOB");
  auto field_DATUM_VZNIKU = finDs->fieldByName("DATUM_VZNIKU");
  auto field_ROK_VZNIKU = finDs->fieldByName("ROK_VZNIKU");
  auto field_DATUM_LIKVIDACE = finDs->fieldByName("DATUM_LIKVIDACE");
  auto field_ROK_LIKVIDACE = finDs->fieldByName("ROK_LIKVIDACE");
  auto field_INSTITUCE_V_LIKVIDACI = finDs->fieldByName("INSTITUCE_V_LIKVIDACI");
  auto field_COPNI_cinnost = finDs->fieldByName("COPNI_cinnost");
  auto field_COPNI_obor = finDs->fieldByName("COPNI_obor");
  auto field_Zdroj = finDs->fieldByName("Zdroj dat");
  auto field_VEREJNA_ZAKAZKA_ID_ZAKAZKY = finDs->fieldByName("VEREJNA_ZAKAZKA_ID_ZAKAZKY");
  auto field_VEREJNA_ZAKAZKA_LIMIT_VZ = finDs->fieldByName("VEREJNA_ZAKAZKA_LIMIT_VZ");
  auto field_VEREJNA_ZAKAZKA_CELKOVA_KONECNA_HODNOTA = finDs->fieldByName("VEREJNA_ZAKAZKA_CELKOVA_KONECNA_HODNOTA");
  auto field_VEREJNA_ZAKAZKA_ZADAVATEL_IC = finDs->fieldByName("VEREJNA_ZAKAZKA_ZADAVATEL_IC");
  auto field_VEREJNA_ZAKAZKA_DATUM_ZADANI = finDs->fieldByName("VEREJNA_ZAKAZKA_DATUM_ZADANI");
  auto field_VEREJNA_ZAKAZKA_NAZEV_VZ = finDs->fieldByName("VEREJNA_ZAKAZKA_NAZEV_VZ");
  auto field_VEREJNA_ZAKAZKA_DRUH_VZ = finDs->fieldByName("VEREJNA_ZAKAZKA_DRUH_VZ");
  auto field_VEREJNA_ZAKAZKA_STRUCNY_POPIS_VZ = finDs->fieldByName("VEREJNA_ZAKAZKA_STRUCNY_POPIS_VZ");
  auto field_VEREJNA_ZAKAZKA_ZADAVATEL_UREDNI_NAZEV = finDs->fieldByName("VEREJNA_ZAKAZKA_ZADAVATEL_UREDNI_NAZEV");
  auto field_VEREJNA_ZAKAZKA_CISLO_CASTI_ZADANI_VZ = finDs->fieldByName("VEREJNA_ZAKAZKA_CISLO_CASTI_ZADANI_VZ");
  auto field_VEREJNA_ZAKAZKA_NAZEV_CASTI_VZ = finDs->fieldByName("VEREJNA_ZAKAZKA_NAZEV_CASTI_VZ");
  auto field_VEREJNA_ZAKAZKA_PREPOKLADANA_CELKOVA_HODNOTA_CASTI_VZ =
      finDs->fieldByName("VEREJNA_ZAKAZKA_PREPOKLADANA_CELKOVA_HODNOTA_CASTI_VZ");
  auto field_DODAVATEL_ICO = finDs->fieldByName("DODAVATEL_ICO");
  auto field_DOTACE_POSKYTOVATEL = finDs->fieldByName("DOTACE_POSKYTOVATEL");
  auto field_DOTACE_CASTKA_UVOLNENA = finDs->fieldByName("DOTACE_CASTKA_UVOLNENA");
  auto field_DOTACE_CASTKA_CERPANA = finDs->fieldByName("DOTACE_CASTKA_CERPANA");
  auto field_OBDOBI = finDs->fieldByName("OBDOBI");
  auto field_ROK_UDELENI_DOTACE = finDs->fieldByName("PROGRAMOVE_OBDOBI");
  auto field_DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU = finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU");
  auto field_DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA = finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA");
  auto field_DOTACE_REGISTRACNI_CISLO_PROJEKTU = finDs->fieldByName("DOTACE_REGISTRACNI_CISLO_PROJEKTU");
  auto field_DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU = finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU");
  auto field_DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU = finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU");
  auto field_DOTACE_OPERACNI_PROGRAM_PRIJEMCE = finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_PRIJEMCE");
  auto field_DOTACE_OPERACNI_PROGRAM_DATUM_SKUTECNE_DATUM_ZAHAJENI_FYZICKE_REALIZACE_PROJEKTU =
      finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_DATUM_SKUTECNE_DATUM_ZAHAJENI_FYZICKE_REALIZACE_PROJEKTU");
  auto field_DOTACE_OPERACNI_PROGRAM_DATUM_REALIZACE_SKUTECNE_DATUM_UKONCENI_FYZICKE_REALIZACE_PROJEKTU =
      finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_DATUM_REALIZACE_SKUTECNE_DATUM_UKONCENI_FYZICKE_REALIZACE_PROJEKTU");
  auto field_DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_KOD =
      finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_KOD");
  auto field_DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_NAZEV =
      finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_NAZEV");
  auto field_DOTACE_OPERACNI_PROGRAM_MISTO_REALIZACE_NUTS3_NAZEV =
      finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_MISTO_REALIZACE_NUTS3_NAZEV");
  auto field_DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_CASTKA =
      finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_CASTKA");
  auto field_DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_EU =
      finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_EU");
  auto field_DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR =
      finDs->fieldByName("DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR");
  auto field_DOTACE_FINANCNI_ZDROJ_KOD = finDs->fieldByName("DOTACE_FINANCNI_ZDROJ_KOD");
  auto field_DOTACE_FINANCNI_ZDROJ_NAZEV = finDs->fieldByName("DOTACE_FINANCNI_ZDROJ_NAZEV");
  auto field_NAZEV_PO = finDs->fieldByName("NAZEV_PO");
  auto field_VEREJNA_SBIRKA_OZNACENI = finDs->fieldByName("VEREJNA_SBIRKA_OZNACENI");
  auto field_VEREJNA_SBIRKA_UZEMNI_ROZSAH = finDs->fieldByName("VEREJNA_SBIRKA_UZEMNI_ROZSAH");
  auto field_VEREJNA_SBIRKA_ZACATEK = finDs->fieldByName("VEREJNA_SBIRKA_ZACATEK");
  auto field_VEREJNA_SBIRKA_KONEC = finDs->fieldByName("VEREJNA_SBIRKA_KONEC");
  auto field_VEREJNA_SBIRKA_ZPUSOB = finDs->fieldByName("VEREJNA_SBIRKA_ZPUSOB");
  auto field_VEREJNA_SBIRKA_UCEL = finDs->fieldByName("VEREJNA_SBIRKA_UCEL");
  auto field_VEREJNA_SBIRKA_UCEL_TEXTEM = finDs->fieldByName("VEREJNA_SBIRKA_UCEL_TEXTEM");
  auto field_download_period = finDs->fieldByName("download_period");
  auto field_X = finDs->fieldByName("X");
  auto field_Y = finDs->fieldByName("Y");

  std::vector<std::string> row;
  finDs->resetBegin();
  auto header = finDs->getFieldNames();
  auto verEndPos = std::find(header.begin(), header.end(), "VEREJNA_SBIRKA_UCEL_TEXTEM");
  header.insert(verEndPos + 1, "VEREJNA_SBIRKA_KONEC_ROK");
  //writer.writeHeader(header);
  std::string zdrojDat;

  const auto columnNames = {
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
      "Zdroj dat"s,
      "VEREJNA_ZAKAZKA_ID_ZAKAZKY"s,
      "VEREJNA_ZAKAZKA_LIMIT_VZ"s,
      "VEREJNA_ZAKAZKA_CELKOVA_KONECNA_HODNOTA"s,
      "VEREJNA_ZAKAZKA_ZADAVATEL_IC"s,
      "VEREJNA_ZAKAZKA_DATUM_ZADANI"s,
      "VEREJNA_ZAKAZKA_NAZEV_VZ"s,
      "VEREJNA_ZAKAZKA_DRUH_VZ"s,
      "VEREJNA_ZAKAZKA_STRUCNY_POPIS_VZ"s,
      "VEREJNA_ZAKAZKA_ZADAVATEL_UREDNI_NAZEV"s,
      "VEREJNA_ZAKAZKA_CISLO_CASTI_ZADANI_VZ"s,
      "VEREJNA_ZAKAZKA_NAZEV_CASTI_VZ"s,
      "VEREJNA_ZAKAZKA_PREPOKLADANA_CELKOVA_HODNOTA_CASTI_VZ"s,
      "DODAVATEL_ICO"s,
      "DOTACE_POSKYTOVATEL"s,
      "DOTACE_CASTKA_UVOLNENA"s,
      "DOTACE_CASTKA_CERPANA"s,
      "OBDOBI"s,
      "PROGRAMOVE_OBDOBI"s,
      "DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU"s,
      "DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA"s,
      "DOTACE_REGISTRACNI_CISLO_PROJEKTU"s,
      "DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU"s,
      "DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU"s,
      "DOTACE_OPERACNI_PROGRAM_PRIJEMCE"s,
      "DOTACE_OPERACNI_PROGRAM_DATUM_SKUTECNE_DATUM_ZAHAJENI_FYZICKE_REALIZACE_PROJEKTU"s,
      "DOTACE_OPERACNI_PROGRAM_DATUM_REALIZACE_SKUTECNE_DATUM_UKONCENI_FYZICKE_REALIZACE_PROJEKTU"s,
      "DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_KOD"s,
      "DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_NAZEV"s,
      "DOTACE_OPERACNI_PROGRAM_MISTO_REALIZACE_NUTS3_NAZEV"s,
      "DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_CASTKA"s,
      "DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_EU"s,
      "DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR"s,
      "DOTACE_FINANCNI_ZDROJ_KOD"s,
      "DOTACE_FINANCNI_ZDROJ_NAZEV"s,
      "NAZEV_PO"s,
      "VEREJNA_SBIRKA_OZNACENI"s,
      "VEREJNA_SBIRKA_UZEMNI_ROZSAH"s,
      "VEREJNA_SBIRKA_ZACATEK"s,
      "VEREJNA_SBIRKA_KONEC"s,
      "VEREJNA_SBIRKA_ZPUSOB"s,
      "VEREJNA_SBIRKA_UCEL"s,
      "VEREJNA_SBIRKA_UCEL_TEXTEM"s,
      "VEREJNA_SBIRKA_KONEC_ROK"s,
      "download_period"s,
      "X"s,
      "Y"s,
  };
  const auto columnTypes = {
      ValueType::Integer, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
  };

  auto resultDS = std::make_shared<DataSets::MemoryDataSet>("dotace");
  resultDS->openEmpty(columnNames, columnTypes);
  auto resultICO_numField = dynamic_cast<DataSets::IntegerField *>(resultDS->fieldByName("ICO_number"));
  auto resultFields = resultDS->getFields();
  // TODO: vratit dataset, ICO_number jako int jinak je to jedno
  ExcelDateTime2DateTimeConverter excelDateTime2DateTimeConverter;
  while (finDs->next()) {
    resultDS->append();
    resultICO_numField->setAsInteger(field_ICO_number->getAsInteger());

    resultFields[1]->setAsString(Utilities::defaultForEmpty(field_ICO->getAsString(), notAvailable));
    resultFields[2]->setAsString(Utilities::defaultForEmpty(field_NAZEV->getAsString(), notAvailable));
    resultFields[3]->setAsString(Utilities::defaultForEmpty(field_PRAVNI_FORMA->getAsString(), notAvailable));
    resultFields[4]->setAsString(Utilities::defaultForEmpty(field_VELIKOSTNI_KATEGORIE->getAsString(), notAvailable));
    resultFields[5]->setAsString(Utilities::defaultForEmpty(field_ADRESA->getAsString(), notAvailable));
    resultFields[6]->setAsString(Utilities::defaultForEmpty(field_KRAJ->getAsString(), notAvailable));
    resultFields[7]->setAsString(Utilities::defaultForEmpty(field_OKRES->getAsString(), notAvailable));
    resultFields[8]->setAsString(Utilities::defaultForEmpty(field_OBEC->getAsString(), notAvailable));
    resultFields[9]->setAsString(Utilities::defaultForEmpty(field_OBEC_OKRES->getAsString(), notAvailable));
    resultFields[10]->setAsString(Utilities::defaultForEmpty(field_OBEC_ICOB->getAsString(), notAvailable));
    resultFields[11]->setAsString(Utilities::defaultForEmpty(field_DATUM_VZNIKU->getAsString(), notAvailable));
    resultFields[12]->setAsString(Utilities::defaultForEmpty(field_ROK_VZNIKU->getAsString(), notAvailable));
    resultFields[13]->setAsString(Utilities::defaultForEmpty(field_DATUM_LIKVIDACE->getAsString(), notAvailable));
    resultFields[14]->setAsString(Utilities::defaultForEmpty(field_ROK_LIKVIDACE->getAsString(), notAvailable));
    resultFields[15]->setAsString(Utilities::defaultForEmpty(field_INSTITUCE_V_LIKVIDACI->getAsString(), notAvailable));
    resultFields[16]->setAsString(Utilities::defaultForEmpty(field_COPNI_cinnost->getAsString(), notAvailable));
    resultFields[17]->setAsString(Utilities::defaultForEmpty(field_COPNI_obor->getAsString(), notAvailable));
    resultFields[18]->setAsString(Utilities::defaultForEmpty(field_Zdroj->getAsString(), notAvailable));


    //row.clear();
    //row.emplace_back(Utilities::defaultForEmpty(field_ICO_number->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_ICO->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_NAZEV->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_PRAVNI_FORMA->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_VELIKOSTNI_KATEGORIE->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_ADRESA->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_KRAJ->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_OKRES->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_OBEC->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_OBEC_OKRES->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_OBEC_ICOB->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_DATUM_VZNIKU->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_ROK_VZNIKU->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_DATUM_LIKVIDACE->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_ROK_LIKVIDACE->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_INSTITUCE_V_LIKVIDACI->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_COPNI_cinnost->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_COPNI_obor->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_Zdroj->getAsString(), notAvailable));
    zdrojDat = field_Zdroj->getAsString();

    if (zdrojDat == "VerejneZakazky") {
      resultFields[19]->setAsString(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_ID_ZAKAZKY->getAsString(), notAvailable));
      resultFields[20]->setAsString(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_LIMIT_VZ->getAsString(), notAvailable));
      resultFields[21]->setAsString(
          Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_CELKOVA_KONECNA_HODNOTA->getAsString(), notAvailable));
      resultFields[22]->setAsString(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_ZADAVATEL_IC->getAsString(), notAvailable));
      auto datumZadani = field_VEREJNA_ZAKAZKA_DATUM_ZADANI->getAsString();
      if (datumZadani != notAvailable) {
        datumZadani = excelDateTime2DateTimeConverter.convert(Utilities::stringToInt(datumZadani)).toString("%d/%m/%Y");
      }
      resultFields[23]->setAsString(datumZadani);
      resultFields[24]->setAsString(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_NAZEV_VZ->getAsString(), notAvailable));
      resultFields[25]->setAsString(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_DRUH_VZ->getAsString(), notAvailable));
      resultFields[26]->setAsString(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_STRUCNY_POPIS_VZ->getAsString(), notAvailable));
      resultFields[27]->setAsString(
          Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_ZADAVATEL_UREDNI_NAZEV->getAsString(), notAvailable));
      resultFields[28]->setAsString(
          Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_CISLO_CASTI_ZADANI_VZ->getAsString(), notAvailable));
      resultFields[29]->setAsString(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_NAZEV_CASTI_VZ->getAsString(), notAvailable));
      resultFields[30]->setAsString(Utilities::defaultForEmpty(
          field_VEREJNA_ZAKAZKA_PREPOKLADANA_CELKOVA_HODNOTA_CASTI_VZ->getAsString(), notAvailable));
      auto dodavatelICO = Utilities::defaultForEmpty(field_DODAVATEL_ICO->getAsString(), notAvailable);
      if (dodavatelICO == "0") {
        resultFields[31]->setAsString(notAvailable);
      } else {
        resultFields[31]->setAsString(dodavatelICO);
      }
      //row.emplace_back(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_ID_ZAKAZKY->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_LIMIT_VZ->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_CELKOVA_KONECNA_HODNOTA->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_ZADAVATEL_IC->getAsString(), notAvailable));
      //auto datumZadani = field_VEREJNA_ZAKAZKA_DATUM_ZADANI->getAsString();
      //if (datumZadani != notAvailable) {
      //  datumZadani = excelDateTime2DateTimeConverter.convert(Utilities::stringToInt(datumZadani)).toString("%d/%m/%Y");
      //}
      //row.emplace_back(datumZadani);
      //row.emplace_back(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_NAZEV_VZ->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_DRUH_VZ->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_STRUCNY_POPIS_VZ->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_ZADAVATEL_UREDNI_NAZEV->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_CISLO_CASTI_ZADANI_VZ->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_VEREJNA_ZAKAZKA_NAZEV_CASTI_VZ->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(
      //    field_VEREJNA_ZAKAZKA_PREPOKLADANA_CELKOVA_HODNOTA_CASTI_VZ->getAsString(), notAvailable));
      //auto dodavatelICO = Utilities::defaultForEmpty(field_DODAVATEL_ICO->getAsString(), notAvailable);
      //if (dodavatelICO == "0") {
      //  row.emplace_back(notAvailable);
      //} else {
      //  row.emplace_back(dodavatelICO);
      //}
    } else {
      //for (auto i : MakeRange::range(13)) {
      //  row.emplace_back(notAvailable);
      //}
      for (auto i : MakeRange::range(19, 32)) {
        resultFields[i]->setAsString(notAvailable);
      }
    }

    if (zdrojDat == "CEDR") {
      resultFields[32]->setAsString(Utilities::defaultForEmpty(field_DOTACE_POSKYTOVATEL->getAsString(), notAvailable));
      resultFields[33]->setAsString(Utilities::defaultForEmpty(field_DOTACE_CASTKA_UVOLNENA->getAsString(), notAvailable));
      resultFields[34]->setAsString(Utilities::defaultForEmpty(field_DOTACE_CASTKA_CERPANA->getAsString(), notAvailable));
      resultFields[35]->setAsString(Utilities::defaultForEmpty(field_OBDOBI->getAsString(), notAvailable));
      resultFields[36]->setAsString(Utilities::defaultForEmpty(field_ROK_UDELENI_DOTACE->getAsString(), notAvailable));
      resultFields[37]->setAsString(
          Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU->getAsString(), notAvailable));
      resultFields[38]->setAsString(
          Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA->getAsString(), notAvailable));
      resultFields[39]->setAsString(
          Utilities::defaultForEmpty(field_DOTACE_REGISTRACNI_CISLO_PROJEKTU->getAsString(), notAvailable));
      resultFields[40]->setAsString(
          Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU->getAsString(), notAvailable));
      resultFields[41]->setAsString(
          Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU->getAsString(), notAvailable));
      resultFields[42]->setAsString(Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_PRIJEMCE->getAsString(), notAvailable));
      resultFields[43]->setAsString(notAvailable);
      resultFields[44]->setAsString(notAvailable);
      resultFields[45]->setAsString(notAvailable);
      resultFields[46]->setAsString(notAvailable);
      resultFields[47]->setAsString(notAvailable);
      resultFields[48]->setAsString(notAvailable);
      resultFields[49]->setAsString(notAvailable);
      resultFields[50]->setAsString(notAvailable);
      resultFields[51]->setAsString(Utilities::defaultForEmpty(field_DOTACE_FINANCNI_ZDROJ_KOD->getAsString(), notAvailable));
      resultFields[52]->setAsString(Utilities::defaultForEmpty(field_DOTACE_FINANCNI_ZDROJ_NAZEV->getAsString(), notAvailable));
      resultFields[53]->setAsString(notAvailable);
      //row.emplace_back(Utilities::defaultForEmpty(field_DOTACE_POSKYTOVATEL->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_DOTACE_CASTKA_UVOLNENA->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_DOTACE_CASTKA_CERPANA->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_OBDOBI->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_ROK_UDELENI_DOTACE->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_DOTACE_REGISTRACNI_CISLO_PROJEKTU->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_PRIJEMCE->getAsString(), notAvailable));
      //row.emplace_back(notAvailable);
      //row.emplace_back(notAvailable);
      //row.emplace_back(notAvailable);
      //row.emplace_back(notAvailable);
      //row.emplace_back(notAvailable);
      //row.emplace_back(notAvailable);
      //row.emplace_back(notAvailable);
      //row.emplace_back(notAvailable);
      //row.emplace_back(Utilities::defaultForEmpty(field_DOTACE_FINANCNI_ZDROJ_KOD->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_DOTACE_FINANCNI_ZDROJ_NAZEV->getAsString(), notAvailable));
      //row.emplace_back(notAvailable);
    } else if (zdrojDat == "DotaceEU") {
      resultFields[32]->setAsString(Utilities::defaultForEmpty(field_DOTACE_POSKYTOVATEL->getAsString(), notAvailable));
      resultFields[33]->setAsString(Utilities::defaultForEmpty(field_DOTACE_CASTKA_UVOLNENA->getAsString(), notAvailable));
      resultFields[34]->setAsString(Utilities::defaultForEmpty(field_DOTACE_CASTKA_CERPANA->getAsString(), notAvailable));
      resultFields[35]->setAsString(Utilities::defaultForEmpty(field_OBDOBI->getAsString(), notAvailable));
      resultFields[36]->setAsString(Utilities::defaultForEmpty(field_ROK_UDELENI_DOTACE->getAsString(), notAvailable));
      resultFields[37]->setAsString(
          Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU->getAsString(), notAvailable));
      resultFields[38]->setAsString(
          Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA->getAsString(), notAvailable));
      resultFields[39]->setAsString(
          Utilities::defaultForEmpty(field_DOTACE_REGISTRACNI_CISLO_PROJEKTU->getAsString(), notAvailable));
      resultFields[40]->setAsString(
          Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU->getAsString(), notAvailable));
      resultFields[41]->setAsString(
          Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU->getAsString(), notAvailable));
      resultFields[42]->setAsString(Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_PRIJEMCE->getAsString(), notAvailable));
      resultFields[43]->setAsString(Utilities::defaultForEmpty(
          field_DOTACE_OPERACNI_PROGRAM_DATUM_SKUTECNE_DATUM_ZAHAJENI_FYZICKE_REALIZACE_PROJEKTU->getAsString(),
          notAvailable));
      resultFields[44]->setAsString(Utilities::defaultForEmpty(
          field_DOTACE_OPERACNI_PROGRAM_DATUM_REALIZACE_SKUTECNE_DATUM_UKONCENI_FYZICKE_REALIZACE_PROJEKTU
              ->getAsString(),
          notAvailable));
      resultFields[45]->setAsString(
          Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_KOD->getAsString(), notAvailable));
      resultFields[46]->setAsString(Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_NAZEV->getAsString(),
                                                                notAvailable));
      resultFields[47]->setAsString(Utilities::defaultForEmpty(
          field_DOTACE_OPERACNI_PROGRAM_MISTO_REALIZACE_NUTS3_NAZEV->getAsString(), notAvailable));
      resultFields[48]->setAsString(Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_CASTKA->getAsString(),
                                                                notAvailable));
      resultFields[49]->setAsString(Utilities::defaultForEmpty(
          field_DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_EU->getAsString(), notAvailable));
      resultFields[50]->setAsString(Utilities::defaultForEmpty(
          field_DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR->getAsString(), notAvailable));
      resultFields[51]->setAsString(notAvailable);
      resultFields[52]->setAsString(notAvailable);
      resultFields[53]->setAsString(Utilities::defaultForEmpty(field_NAZEV_PO->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_DOTACE_POSKYTOVATEL->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_DOTACE_CASTKA_UVOLNENA->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_DOTACE_CASTKA_CERPANA->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_OBDOBI->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_ROK_UDELENI_DOTACE->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_DOTACE_REGISTRACNI_CISLO_PROJEKTU->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU->getAsString(), notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_PRIJEMCE->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(
      //    field_DOTACE_OPERACNI_PROGRAM_DATUM_SKUTECNE_DATUM_ZAHAJENI_FYZICKE_REALIZACE_PROJEKTU->getAsString(),
      //    notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(
      //    field_DOTACE_OPERACNI_PROGRAM_DATUM_REALIZACE_SKUTECNE_DATUM_UKONCENI_FYZICKE_REALIZACE_PROJEKTU
      //        ->getAsString(),
      //    notAvailable));
      //row.emplace_back(
      //    Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_KOD->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_NAZEV->getAsString(),
      //                                            notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(
      //    field_DOTACE_OPERACNI_PROGRAM_MISTO_REALIZACE_NUTS3_NAZEV->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_CASTKA->getAsString(),
      //                                            notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(
      //    field_DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_EU->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(
      //    field_DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR->getAsString(), notAvailable));
      //row.emplace_back(notAvailable);
      //row.emplace_back(notAvailable);
      //row.emplace_back(Utilities::defaultForEmpty(field_NAZEV_PO->getAsString(), notAvailable));
    } else {
      //for (auto i : MakeRange::range(22)) {
      //  row.emplace_back(notAvailable);
      //}
      for (auto i : MakeRange::range(32, 54)) {
        resultFields[i]->setAsString(notAvailable);
      }
    }

    if (zdrojDat == "VerejnaSbirka") {
      resultFields[54]->setAsString(Utilities::defaultForEmpty(field_VEREJNA_SBIRKA_OZNACENI->getAsString(), notAvailable));
      resultFields[55]->setAsString(Utilities::defaultForEmpty(field_VEREJNA_SBIRKA_UZEMNI_ROZSAH->getAsString(), notAvailable));
      auto verSBZacatek = field_VEREJNA_SBIRKA_ZACATEK->getAsString();
      verSBZacatek = excelDateTime2DateTimeConverter.convert(Utilities::stringToInt(verSBZacatek)).toString("%d/%m/%Y");
      auto verSBKonec = field_VEREJNA_SBIRKA_KONEC->getAsString();
      std::string konecVSRok;
      if (verSBKonec != notAvailable) {
        verSBKonec = excelDateTime2DateTimeConverter.convert(Utilities::stringToInt(verSBKonec)).toString("%d/%m/%Y");
        konecVSRok = verSBKonec.substr(verSBKonec.size() - 4);
      } else {
        konecVSRok = notAvailable;
      }
      resultFields[56]->setAsString(Utilities::defaultForEmpty(verSBZacatek, notAvailable));
      resultFields[57]->setAsString(Utilities::defaultForEmpty(verSBKonec, notAvailable));
      resultFields[58]->setAsString(Utilities::defaultForEmpty(field_VEREJNA_SBIRKA_ZPUSOB->getAsString(), notAvailable));
      resultFields[59]->setAsString(Utilities::defaultForEmpty(field_VEREJNA_SBIRKA_UCEL->getAsString(), notAvailable));
      resultFields[60]->setAsString(Utilities::defaultForEmpty(field_VEREJNA_SBIRKA_UCEL_TEXTEM->getAsString(), notAvailable));
      resultFields[61]->setAsString(konecVSRok);
      //row.emplace_back(Utilities::defaultForEmpty(field_VEREJNA_SBIRKA_OZNACENI->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_VEREJNA_SBIRKA_UZEMNI_ROZSAH->getAsString(), notAvailable));
      //auto verSBZacatek = field_VEREJNA_SBIRKA_ZACATEK->getAsString();
      //verSBZacatek = excelDateTime2DateTimeConverter.convert(Utilities::stringToInt(verSBZacatek)).toString("%d/%m/%Y");
      //auto verSBKonec = field_VEREJNA_SBIRKA_KONEC->getAsString();
      //std::string konecVSRok;
      //if (verSBKonec != notAvailable) {
      //  verSBKonec = excelDateTime2DateTimeConverter.convert(Utilities::stringToInt(verSBKonec)).toString("%d/%m/%Y");
      //  konecVSRok = verSBKonec.substr(verSBKonec.size() - 4);
      //} else {
      //  konecVSRok = notAvailable;
      //}
      //row.emplace_back(Utilities::defaultForEmpty(verSBZacatek, notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(verSBKonec, notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_VEREJNA_SBIRKA_ZPUSOB->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_VEREJNA_SBIRKA_UCEL->getAsString(), notAvailable));
      //row.emplace_back(Utilities::defaultForEmpty(field_VEREJNA_SBIRKA_UCEL_TEXTEM->getAsString(), notAvailable));
      //row.emplace_back(konecVSRok);
    } else {
      //for (auto i : MakeRange::range(8)) {
      //  row.emplace_back(notAvailable);
      //}
      for (auto i : MakeRange::range(54, 62)) {
        resultFields[i]->setAsString(notAvailable);
      }
    }

    if (zdrojDat == "VerejnaSbirka") {
      std::string rok = field_VEREJNA_SBIRKA_ZACATEK->getAsString();
      if (rok != notAvailable) {
        rok = excelDateTime2DateTimeConverter.convert(Utilities::stringToInt(rok)).toString("%Y");
      }
      resultFields[35]->setAsString(Utilities::defaultForEmpty(rok, notAvailable));
    } else if (zdrojDat == "VerejneZakazky") {
      std::string rok = field_VEREJNA_ZAKAZKA_DATUM_ZADANI->getAsString();
      if (rok != notAvailable) {
        rok = excelDateTime2DateTimeConverter.convert(Utilities::stringToInt(rok)).toString("%Y");
      }
      resultFields[35]->setAsString(Utilities::defaultForEmpty(rok, notAvailable));
    } else if (zdrojDat == "DotaceEU") {
      std::string rok = field_ROK_UDELENI_DOTACE->getAsString();
      resultFields[35]->setAsString(Utilities::defaultForEmpty(rok, notAvailable));
      resultFields[36]->setAsString(field_OBDOBI->getAsString());
    }

    resultFields[62]->setAsString(Utilities::defaultForEmpty(field_download_period->getAsString(), notAvailable));
    resultFields[63]->setAsString(Utilities::defaultForEmpty(field_X->getAsString(), notAvailable));
    resultFields[64]->setAsString(Utilities::defaultForEmpty(field_Y->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_download_period->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_X->getAsString(), notAvailable));
    //row.emplace_back(Utilities::defaultForEmpty(field_Y->getAsString(), notAvailable));

  }

  logger.endTime();
  logger.printElapsedTime();
  return resultDS;
}
