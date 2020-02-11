//
// Created by petr on 10/7/19.
//

#include "Combiner.h"
#include <CsvReader.h>
#include <CsvStreamReader.h>
#include <CsvWriter.h>
#include <Logger.h>
#include <MemoryDataBase.h>
#include <MemoryDataSet.h>
#include <XlntReader.h>
#include <XlntWriter.h>
#include <XlsxIOReader.h>
#include <XlsxReader.h>
#include <memory>
#include "time/now.h"
#include "fmt/format.h"
using namespace std::string_literals;

const auto notAvailable = "není k dispozici"s;
auto &logger = Logger<true>::GetInstance();

struct FileSettings {
  enum Type { csv, xlsx, xls, csvOld };
  Type type;
  std::string pathToFile;
  char delimiter;
  std::string sheet = "";
  bool useCharset = false;
  CharSet charset;
  bool useQuotes = true;

  static FileSettings Xlsx(const std::string &path, const std::string &sheet = "") {
    FileSettings result;
    result.type = xlsx;
    result.pathToFile = path;
    result.sheet = sheet;
    return result;
  }

  static FileSettings Xls(const std::string &path, const std::string &sheet = "") {
    FileSettings result;
    result.type = xls;
    result.pathToFile = path;
    result.sheet = sheet;
    return result;
  }

  static FileSettings Csv(const std::string &path, char delimiter = ',', bool useCharset = false,
                          CharSet charset = CharSet::CP1250) {
    FileSettings result;
    result.type = csv;
    result.pathToFile = path;
    result.delimiter = delimiter;
    result.useCharset = useCharset;
    result.charset = charset;
    return result;
  }

  static FileSettings CsvOld(const std::string &path, char delimiter = ',', bool useQuotes = true,
                             bool useCharset = false, CharSet charset = CharSet::CP1250) {
    FileSettings result;
    result.type = csvOld;
    result.pathToFile = path;
    result.delimiter = delimiter;
    result.useCharset = useCharset;
    result.useQuotes = useQuotes;
    result.charset = charset;
    return result;
  }
};

std::shared_ptr<DataSets::MemoryDataSet> createDataSetFromFile(const std::string &name,
                                                               const FileSettings &fileSettings,
                                                               const std::vector<ValueType> &valueTypes) {
  std::unique_ptr<DataProviders::BaseDataProvider> provider;
  switch (fileSettings.type) {
  case FileSettings::csv:
    if (fileSettings.useCharset) {
      provider = std::make_unique<DataProviders::CsvStreamReader>(fileSettings.pathToFile, fileSettings.charset,
                                                                  fileSettings.delimiter);
    } else {
      provider = std::make_unique<DataProviders::CsvStreamReader>(fileSettings.pathToFile, fileSettings.delimiter);
    }
    break;
  case FileSettings::csvOld:
    if (fileSettings.useCharset) {
      provider =
          std::make_unique<DataProviders::CsvReader>(fileSettings.pathToFile, fileSettings.charset,
                                                     std::string(1, fileSettings.delimiter), fileSettings.useQuotes);
    } else {
      provider = std::make_unique<DataProviders::CsvReader>(
          fileSettings.pathToFile, std::string(1, fileSettings.delimiter), fileSettings.useQuotes);
    }
    break;
  case FileSettings::xlsx:
    provider = std::make_unique<DataProviders::XlsxIOReader>(fileSettings.pathToFile);
    if (!fileSettings.sheet.empty()) {
      dynamic_cast<DataProviders::XlsxIOReader *>(provider.get())->openSheet(fileSettings.sheet);
    }
    break;
  case FileSettings::xls:
    provider = std::make_unique<DataProviders::XlsxReader>(fileSettings.pathToFile);
    break;
  }
  auto result = std::make_shared<DataSets::MemoryDataSet>(name);
  result->open(*provider, valueTypes);
  logger.log<LogLevel::Status>("Loaded: "s + result->getName());
  result->resetEnd();
  logger.log<LogLevel::Debug, true>("Count:", result->getCurrentRecord());
  result->resetBegin();
  return result;
}

const auto verejneSbirkyPath = "/home/petr/Desktop/muni/verejnesbirky/"s;
const auto muniPath = "/home/petr/Desktop/muni/"s;
const auto dotacePath = "/home/petr/Desktop/muni/dotaceeu/"s;
const auto verejneZakazkyPath = "/home/petr/Desktop/muni/verejnezakazky/"s;
const auto cedrPath = "/home/petr/Desktop/muni/cedr/out/"s;
const std::string velkatCSV = muniPath + "/velikostni_kategorie.csv";

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
void combine(std::vector<std::shared_ptr<DataSets::BaseDataSet>> dataSets, const std::vector<TableColumn>& joins,
             const std::string& dest) {
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

  std::vector<std::string> row;

  DataWriters::CsvWriter writer(dest);

  std::transform(mainDS.fields.begin(), mainDS.fields.end(), std::back_inserter(row),
                 [](auto &field) { return field->getName().data(); });
  row.emplace_back("Zdroj dat");
  dss = std::vector<DSFieldCnt>{dss.begin() + 1, dss.end()};
  for (auto &ds : dss) {
    std::transform(ds.fields.begin(), ds.fields.end(), std::back_inserter(row),
                   [](auto &field) { return field->getName().data(); });
  }
  row.emplace_back("download_period");

  writer.writeHeader(row);
  writer.setAddQuotes(true);

  auto cnt = 0;

  const auto currentTime = std::chrono::system_clock::now();
  auto now_t = std::chrono::system_clock::to_time_t(currentTime);
  struct tm *parts = std::localtime(&now_t);
  const auto year = 1900 + parts->tm_year;
  const auto month = 1 + parts->tm_mon;
  const auto downloadPeriodData = fmt::format("{:04d}-{:02d}", year, month);
  auto likvField = mainDS.ds->fieldByName("DATUM_LIKVIDACE");
  while (mainDS.ds->next()) {

    if (likvField->getAsString().empty()) {
      likvField->setAsString("Aktivní");
    }

    ++cnt;
    for (auto &ds : dss) {
      ds.ds->setCurrentRecord(ds.pos);
      while (ds.ds->next()) {
        if (mainField->getAsInteger() == ds.field->getAsInteger()) {
          row.clear();
          std::transform(mainDS.fields.begin(), mainDS.fields.end(), std::back_inserter(row),
                         [](auto &field) { return field->getAsString(); });
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
                         [](auto &field) { return field->getAsString(); });

          fill = rowLength - row.size();
          for (int i = 0; i < fill; ++i) {
            row.emplace_back(notAvailable);
          }
          row.emplace_back(downloadPeriodData);
          writer.writeRecord(row);
        } else if (mainField->getAsInteger() < ds.field->getAsInteger()) {
          ds.pos = ds.ds->getCurrentRecord() - 1;
          break;
        }
      }
    }
  }
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

std::shared_ptr<DataSets::MemoryDataSet> transformCedr(const std::shared_ptr<DataSets::BaseDataSet>& cedr) {
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
        //logger.log<LogLevel::Debug>(resultFields[cnt]->getName());
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

std::shared_ptr<DataSets::MemoryDataSet> appendCedrDotace(const std::shared_ptr<DataSets::BaseDataSet>& cedr,
                                                          const std::shared_ptr<DataSets::BaseDataSet>& dotace) {
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

int main() {
  logger.startTime();
  DataBase::MemoryDataBase db("db");

  auto verejneSbirkyDS = createDataSetFromFile(
      "verejneSbirky", FileSettings::CsvOld(verejneSbirkyPath + "VerejneSbirky.csv", '|', false, true, CharSet::CP1250),
      {
          ValueType::String,
          ValueType::String,
          ValueType::String,
          ValueType::String,
          ValueType::String,
          ValueType::String,
          ValueType::String,
          ValueType::String,
          ValueType::String,
      });
  db.addTable(verejneSbirkyDS);

  auto subjekty = createDataSetFromFile("subjekty", FileSettings::Xlsx(muniPath + "NNO.xlsx"),
                                        {ValueType::Integer, ValueType::Integer, ValueType::String, ValueType::String,
                                         ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                         ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                         ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                         ValueType::String});
  db.addTable(subjekty);
  db.addTable(createDataSetFromFile("velkat", {FileSettings::Type::csv, velkatCSV, ','},
                                    {ValueType::String, ValueType::String}));

  const std::string katQuery = "select subjekty.*, velkat.velikostni_kategorie_index from subjekty join velkat on "
                               "subjekty.Velikostní_kategorie = velkat.TEXT;";
  auto vel = db.execSimpleQuery(katQuery, false, "subjekty");
  auto newNno = vel->dataSet->toDataSet();
  db.removeTable("subjekty");
  db.addTable(newNno);

  db.execSimpleQuery("select verejneSbirky.NAZEV_PO, verejneSbirky.OZNACENI_SBIRKY AS VEREJNA_SBIRKA_OZNACENI, "
                     "verejneSbirky.UZEMNI_ROZSAH AS "
                     "VEREJNA_SBIRKA_UZEMNI_ROZSAH, verejneSbirky.DATUM_ZAHAJENI AS VEREJNA_SBIRKA_ZACATEK, "
                     "verejneSbirky.DATUM_UKONCENI AS VEREJNA_SBIRKA_KONEC, verejneSbirky.ZPUSOB_PROVADENI AS "
                     "VEREJNA_SBIRKA_ZPUSOB, verejneSbirky.UCEL_SBIRKY AS VEREJNA_SBIRKA_UCEL, "
                     "verejneSbirky.UCEL_TEXTEM AS VEREJNA_SBIRKA_UCEL_TEXTEM, subjekty.ICOnum "
                     "from verejneSbirky "
                     "JOIN subjekty on verejneSbirky.NAZEV_PO = subjekty.NAZEV;",
                     true, "verSb");

  db.execSimpleQuery("select subjekty.* from subjekty;", true, "sub");

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

  const std::string query = "select VZ.EvidencniCisloVZnaVVZ AS VEREJNA_ZAKAZKA_ID_ZAKAZKY, "
                            "VZ.LimitVZ AS VEREJNA_ZAKAZKA_LIMIT_VZ, "
                            //"VZ.OdhadovanaHodnotaVZbezDPH, "
                            "VZ.CelkovaKonecnaHodnotaVZ AS VEREJNA_ZAKAZKA_CELKOVA_KONECNA_HODNOTA, "
                            "VZ.ZadavatelICO AS VEREJNA_ZAKAZKA_ZADAVATEL_IC, "
                            "VZ.DatumUverejneni AS VEREJNA_ZAKAZKA_DATUM_ZADANI, "
                            "VZ.NazevVZ AS VEREJNA_ZAKAZKA_NAZEV_VZ, "
                            "VZ.DruhVZ AS VEREJNA_ZAKAZKA_DRUH_VZ, "
                            "VZ.StrucnyPopisVZ AS VEREJNA_ZAKAZKA_STRUCNY_POPIS_VZ, "
                            "VZ.ZadavatelUredniNazev AS VEREJNA_ZAKAZKA_ZADAVATEL_UREDNI_NAZEV, "
                            "castiVZ.ID_CastiVZ AS VEREJNA_ZAKAZKA_CISLO_CASTI_ZADANI_VZ, "
                            "castiVZ.NazevCastiVZ AS VEREJNA_ZAKAZKA_NAZEV_CASTI_VZ, dodVZ.DodavatelICO from VZ "
                            "join castiVZ on VZ.ID_Zakazky = castiVZ.ID_Zakazky "
                            "join dodVZ on VZ.ID_Zakazky = dodVZ.ID_Zakazky "
                            "join subjekty on dodVZ.DodavatelICO = subjekty.ICOnum;";

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
                                     //"cedr.castkaUvolnena AS DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR, "
                                     //"cedr.dotaceTitulNazev, " "cedr.projektIdnetifikator, "
                                     "cedr.ICOnum, cedr.financniZdrojKod AS DOTACE_FINANCNI_ZDROJ_KOD, "
                                     "cedr.financniZdrojNazev AS DOTACE_FINANCNI_ZDROJ_NAZEV from cedr;",
                                     false, "cedrView");
  auto cedrReady = transformCedr(cedrView->dataSet);
  // db.addTable(cedrReady);
  // db.execSimpleQuery("select cedrReady.* from cedrReady;", true, "cedr2");

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
      createDataSetFromFile("dotace_dopl", FileSettings::Xlsx(muniPath+ "NNO_doplneni.xlsx", "dotaceeu_poskytovatel"),
                            {ValueType::String, ValueType::String});
  db.addTable(dotaceDoplneniDS);
  auto dotaceSub = db.execSimpleQuery("select dotace.*, "
                                      "subjekty.ICOnum, dotace_dopl.Poskytovatel from dotace "
                                      "join subjekty on dotace.IC = subjekty.ICOnum "
                                      "left join dotace_dopl on dotace.Program = dotace_dopl.Program;",
                                      false, "dotaceSub");

  dotaceSub->dataSet->resetEnd();
  logger.log<LogLevel::Info>("Query dotace2done: ", dotaceSub->dataSet->getCurrentRecord());
  dotaceSub->dataSet->resetBegin();

  auto dotace2 = transformDotace(dotaceSub->dataSet);
  // db.addTable(dotace2);
  // db.execSimpleQuery("select dotace2.* from dotace2;", true, "dotace2");
  db.execSimpleQuery("select vz.* from vz join subjekty on vz.DodavatelICO = subjekty.ICOnum;", true, "vz2");
  logger.log<LogLevel::Info>("Query vz2 done");

  auto wtf = appendCedrDotace(cedrReady, dotace2);
  db.addTable(wtf);
  auto cdtc = db.execSimpleQuery("select cedrDotace.* from cedrDotace;", true, "cedrDotace");
  cdtc->dataSet->resetEnd();
  logger.log<LogLevel::Info>("Query cedrEU: ", cdtc->dataSet->getCurrentRecord());
  cdtc->dataSet->resetBegin();

  /*{
    DataWriters::CsvWriter w("/home/petr/Desktop/t.csv");
    w.setAddQuotes(true);
    w.writeDataSet(*cdtc->dataSet);
  }*/

  combine({db.viewByName("sub")->dataSet->toDataSet(), db.viewByName("vz2")->dataSet, cdtc->dataSet,
           db.viewByName("verSb")->dataSet}
          /*{db.viewByName("sub")->dataSet, db.viewByName("vz2")->dataSet, db.viewByName("cedr2")->dataSet,
               db.viewByName("dotace2")->dataSet, db.viewByName("verSb")->dataSet}*/
          ,
          {{"sub", "ICOnum"},
           {"vz2", "DodavatelICO"},
           {"cedrDotace", "DOTACE_OPERACNI_PROGRAM_PRIJEMCE"},
           //{"cedr2", "ICOnum"},
           //{"dotace2", "DOTACE_OPERACNI_PROGRAM_PRIJEMCE"},
           {"verSb", "ICOnum"}},
          muniPath + "export2.csv");

  logger.endTime();
  logger.printElapsedTime();
  return 0;
}