//
// Created by petr on 2/11/20.
//

#include "Misc/Headers/Logger.h"
#include <CsvReader.h>
#include <CsvStreamReader.h>
#include <CsvWriter.h>
#include <MemoryDataBase.h>
#include <XlsxIOReader.h>
#include <XlsxReader.h>
#include <include/fmt/format.h>
#include "various/isin.h"
using namespace std::string_literals;

auto &logger = Logger<true>::GetInstance();

struct FileSettings {
  enum Type { csv, xlsx, xls, csvOld };
  Type type;
  std::string pathToFile;
  char delimiter;
  std::string sheet = "";
  bool useCharset = false;
  CharSet charset;

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

  static FileSettings CsvOld(const std::string &path, char delimiter = ',', bool useCharset = false,
                             CharSet charset = CharSet::CP1250) {
    FileSettings result;
    result.type = csvOld;
    result.pathToFile = path;
    result.delimiter = delimiter;
    result.useCharset = useCharset;
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
      provider = std::make_unique<DataProviders::CsvReader>(fileSettings.pathToFile, fileSettings.charset,
                                                            std::string(1, fileSettings.delimiter), true);
    } else {
      provider = std::make_unique<DataProviders::CsvReader>(fileSettings.pathToFile,
                                                            std::string(1, fileSettings.delimiter), true);
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
const auto notAvailable = "není k dispozici"s;
std::shared_ptr<DataSets::MemoryDataSet> prepareNACE() {
  DataBase::MemoryDataBase db("db");
  db.addTable(
      createDataSetFromFile("icos", FileSettings::Csv("/home/petr/Desktop/muni/icos.csv"), {ValueType::Integer}));
  db.addTable(createDataSetFromFile(
      "res_nace", FileSettings::Csv("/home/petr/Desktop/muni/res/all/RES3B2020M02.csv", ';', true, CharSet::CP1250),
      {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String}));
  db.addTable(createDataSetFromFile(
      "nace",
      FileSettings::Csv("/home/petr/Desktop/muni/res/all/DM_ERES_ERES_RES_U_CIS_80004.csv", ';', true, CharSet::CP1250),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String}));
  auto ordered = db.execSimpleQuery(R"(SELECT res_nace.ICO, res_nace.HODN, nace.TEXTZ
FROM res_nace
JOIN icos ON res_nace.ICO = icos.ICO
LEFT JOIN nace ON res_nace.HODN = nace.KODZAZ
ORDER BY res_nace.ICO ASC;)",
                                    false, "nace_ordered");
  auto result = std::make_shared<DataSets::MemoryDataSet>("nace");
  result->openEmpty({"ICO", "KOD_NACE", "CINNOSTI_DLE_NACE"},
                    {ValueType::Integer, ValueType::String, ValueType::String});
  int lastIco = 0;
  ordered->dataSet->resetBegin();

  auto icoField = dynamic_cast<DataSets::IntegerField *>(ordered->dataSet->fieldByName("ICO"));
  auto kodField = ordered->dataSet->fieldByName("HODN");
  auto textField = ordered->dataSet->fieldByName("TEXTZ");

  auto result_icoField = dynamic_cast<DataSets::IntegerField *>(result->fieldByName("ICO"));
  auto result_kodField = result->fieldByName("KOD_NACE");
  auto result_textField = result->fieldByName("CINNOSTI_DLE_NACE");

  while (ordered->dataSet->next()) {
    const auto currentIco = icoField->getAsInteger();
    if (currentIco != lastIco) {
      if (lastIco != 0) {
        if (result_kodField->getAsString().empty()) {
          result_kodField->setAsString(notAvailable);
          result_textField->setAsString(notAvailable);
        } else {
          result_kodField->setAsString(result_kodField->getAsString().substr(1));
          result_textField->setAsString(result_textField->getAsString().substr(1));
        }
      }
      result->append();
      result_icoField->setAsInteger(currentIco);
      result_kodField->setAsString("");
      result_textField->setAsString("");
      lastIco = currentIco;
    }
    const auto kodValue = kodField->getAsString();
    if (!kodValue.empty() && kodValue != notAvailable) {
      result_kodField->setAsString(result_kodField->getAsString() + "|" + kodField->getAsString());
      result_textField->setAsString(result_textField->getAsString() + "|" + textField->getAsString());
    }
  }

  return result;
}
// OBEC_TEXT, PSC, COBCE_TEXT, ULICE_TEXT CDOM/COR pokud prazdne
std::string buildAdresa(const std::string &obec, const std::string &psc, const std::string &castObce,
                        const std::string &ulice, const std::string &cisloDomu, const std::string &cisloOrientacni) {
  std::string result = fmt::format("{}, {}", obec, psc);
  bool hadUliceOrCastObce = false;
  if (!castObce.empty()) {
    hadUliceOrCastObce = true;
    result += fmt::format(", {}", castObce);
  }
  if (!ulice.empty()) {
    hadUliceOrCastObce = true;
    result += fmt::format(", {}", ulice);
  }
  if (!hadUliceOrCastObce) {
    result += fmt::format(", {}", obec);
  }
  result += fmt::format(" {}", cisloDomu);
  if (!cisloOrientacni.empty()) {
    result += fmt::format("/{}", cisloOrientacni);
  }

  return result;
};

int main() {
  logger.startTime();
  DataBase::MemoryDataBase db("db");

  db.addTable(prepareNACE());

  db.addTable(
      createDataSetFromFile("icos", FileSettings::Csv("/home/petr/Desktop/muni/icos.csv"), {ValueType::Integer}));

  db.addTable(createDataSetFromFile(
      "forma",
      FileSettings::Csv("/home/petr/Desktop/muni/res/all/DM_ERES_ERES_RES_U_CIS_149.csv", ';', true, CharSet::CP1250),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String}));

  db.addTable(createDataSetFromFile(
      "katpo",
      FileSettings::Csv("/home/petr/Desktop/muni/res/all/DM_ERES_ERES_RES_U_CIS_579.csv", ';', true, CharSet::CP1250),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String}));

  db.addTable(createDataSetFromFile(
      "okreslau",
      FileSettings::Csv("/home/petr/Desktop/muni/res/all/DM_ERES_ERES_RES_U_CIS_109.csv", ';', true, CharSet::CP1250),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String}));

  db.addTable(createDataSetFromFile("rso", FileSettings::Csv("/home/petr/Desktop/muni/res/rso.csv", ','),
                                    {ValueType::String, ValueType::String, ValueType::String, ValueType::String}));

  db.addTable(createDataSetFromFile(
      "res", FileSettings::Csv("/home/petr/Desktop/muni/res/all/RES3A2020M02.csv", ';', true, CharSet::CP1250),
      {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String,  ValueType::String, ValueType::String, ValueType::String}));
  const auto queryFilterIcos = R"(SELECT res.* from res join icos on res.ICO = icos.ICO;)";
  db.addTable(db.execSimpleQuery(queryFilterIcos, false, "resFiltered")->dataSet->toDataSet());

  db.removeTable("icos");
  db.removeTable("res");

  // 19015291

  const auto queryCisleniky = R"sql(SELECT
resFiltered.ICO,
resFiltered.FIRMA AS NAZEV,
forma.TEXTZ AS PRAVNI_FORMA,
katpo.TEXTZ AS VELIKOSTNI_KATEGORIE,
resFiltered.TEXTADR as ADRESA,
rso.KRAJ,
okreslau.TEXTZ as OKRES,
resFiltered.OBEC_TEXT as OBEC,
rso.KOD_OBEC as OBEC_ICOB,
resFiltered.DDATVZN AS DATUM_VZNIKU,
resFiltered.DDATZAN AS DATUM_ZANIKU,
resFiltered.PSC,
resFiltered.COBCE_TEXT,
resFiltered.ULICE_TEXT,
resFiltered.CDOM,
resFiltered.COR,
nace.CINNOSTI_DLE_NACE
FROM resFiltered
LEFT JOIN nace on resFiltered.ICO = nace.ICO
LEFT JOIN forma ON resFiltered.FORMA = forma.KODZAZ
LEFT JOIN katpo ON resFiltered.KATPO = katpo.KODZAZ
LEFT JOIN okreslau ON resFiltered.OKRESLAU = okreslau.KODZAZ
LEFT JOIN rso on resFiltered.ICZUJ = rso.KOD_ZUJ
ORDER BY resFiltered.ICO ASC
;)sql";

  // resFiltered.DDATVZN AS ROK_VZNIKU,
  // resFiltered.DDATZAN AS ROK_ZANIKU,
  // resFiltered.DDATZAN AS INSTITUCE_V_LIKVIDACI

  {
    std::vector<std::string> okresniMesta{};
    {
      DataProviders::CsvReader csvReader("/home/petr/Desktop/muni/res/okresni_mesta.csv");
      std::transform(csvReader.begin(), csvReader.end(), std::back_inserter(okresniMesta), [] (const auto &row) {
        std::cout << row[0] << std::endl;
        return row[0];
      });
    }
    auto result = db.execSimpleQuery(queryCisleniky, false, "result")->dataSet;
    auto ICO_field = dynamic_cast<DataSets::IntegerField*>(result->fieldByName("ICO")); // ICOnum
    // ICO 8mistne
    auto NAZEV_field = result->fieldByName("NAZEV");
    auto PRAVNI_FORMA_field = result->fieldByName("PRAVNI_FORMA");
    auto VELIKOSTNI_KATEGORIE_field = result->fieldByName("VELIKOSTNI_KATEGORIE");
    auto ADRESA_field =
        result->fieldByName("ADRESA"); // bude slozeno z  OBEC_TEXT, PSC, COBCE_TEXT, ULICE_TEXT CDOM/COR pokud prazdne
    auto KRAJ_field = result->fieldByName("KRAJ");
    auto OKRES_field = result->fieldByName("OKRES");
    auto OBEC_field = result->fieldByName("OBEC");
    auto OBEC_ICOB_field = result->fieldByName("OBEC_ICOB");
    auto DATUM_VZNIKU_field =
        result->fieldByName("DATUM_VZNIKU"); // potreba predelat na normalni datum, zkratit a nahradit - /
    // ROK_VZNIKU
    auto DATUM_ZANIKU_field =
        result->fieldByName("DATUM_ZANIKU"); // potreba predelat na normalni datum, zkratit a nahradit - /
    // ROK_ZANIKU
    // INSTITUCE_V_LIKVIDACI - pokud je datum zaniku prazdne tak aktivní, jinak rok zaniku
    auto PSC_field = result->fieldByName("PSC");
    auto COBCE_TEXT_field = result->fieldByName("COBCE_TEXT");
    auto ULICE_TEXT_field = result->fieldByName("ULICE_TEXT");
    auto CDOM_field = result->fieldByName("CDOM");
    auto COR_field = result->fieldByName("COR");
    auto CINNOSTI_DLE_NACE_field = result->fieldByName("CINNOSTI_DLE_NACE");

    DataWriters::CsvWriter writer{"/home/petr/Desktop/test.csv"};
    writer.setAddQuotes(true);

    result->resetBegin();
    writer.writeHeader({"ICOnum", "ICO", "NAZEV", "PRAVNI_FORMA", "VELIKOSTNI_KATEGORIE", "ADRESA", "KRAJ", "OKRES",
                        "OBEC", "OBEC_ICOB", "DATUM_VZNIKU", "ROK_VZNIKU", "DATUM_ZANIKU", "ROK_ZANIKU",
                        "INSTITUCE_V_LIKVIDACI", "CINNOSTI_DLE_NACE"});
    std::vector<std::string> record;
    while (result->next()) {
      record.clear();
      record.emplace_back(ICO_field->getAsString());
      record.emplace_back(fmt::format("{:08d}", ICO_field->getAsInteger()));
      record.emplace_back(Utilities::defaultForEmpty(NAZEV_field->getAsString(), notAvailable));
      record.emplace_back(Utilities::defaultForEmpty(PRAVNI_FORMA_field->getAsString(), notAvailable));
      record.emplace_back(Utilities::defaultForEmpty(VELIKOSTNI_KATEGORIE_field->getAsString(), notAvailable));
      auto adresa = ADRESA_field->getAsString();
      if (adresa.empty()) {
        adresa = buildAdresa(OBEC_field->getAsString(), PSC_field->getAsString(), COBCE_TEXT_field->getAsString(),
                             ULICE_TEXT_field->getAsString(), CDOM_field->getAsString(), COR_field->getAsString());
      }
      record.emplace_back(Utilities::defaultForEmpty(adresa, notAvailable));
      record.emplace_back(Utilities::defaultForEmpty(KRAJ_field->getAsString(), notAvailable));
      record.emplace_back(Utilities::defaultForEmpty(OKRES_field->getAsString(), notAvailable));
      auto obec = OBEC_field->getAsString();
      if (obec.empty()) {
        record.emplace_back(notAvailable);
      } else if (isIn(obec, okresniMesta)) {
        record.emplace_back(obec);
      } else {
        record.emplace_back(fmt::format("{} ({})", obec, OKRES_field->getAsString()));
      }
      record.emplace_back(Utilities::defaultForEmpty(OBEC_ICOB_field->getAsString(), notAvailable));
      auto datumVzniku = DATUM_VZNIKU_field->getAsString();
      if (!datumVzniku.empty()) {
        datumVzniku = datumVzniku.substr(0, 10);
        const auto rokVzniku = datumVzniku.substr(0, 4);
        const auto mesicVzniku = datumVzniku.substr(5, 2);
        const auto denVzniku = datumVzniku.substr(8, 2);
        record.emplace_back(fmt::format("{}/{}/{}", denVzniku, mesicVzniku, rokVzniku));
        record.emplace_back(rokVzniku);
      } else {
        record.emplace_back(notAvailable);
        record.emplace_back(notAvailable);
      }
      auto datumZaniku = DATUM_ZANIKU_field->getAsString();
      auto rokZaniku = ""s;
      if (!datumZaniku.empty()) {
        datumZaniku = datumZaniku.substr(0, 10);
        rokZaniku = datumZaniku.substr(0, 4);
        const auto mesicZaniku = datumZaniku.substr(5, 2);
        const auto denZaniku = datumZaniku.substr(8, 2);
        record.emplace_back(fmt::format("{}/{}/{}", denZaniku, mesicZaniku, rokZaniku));
        record.emplace_back(rokZaniku);
      } else {
        record.emplace_back("aktivní");
        record.emplace_back("aktivní");
      }
      if (rokZaniku.empty()) {
        record.emplace_back("aktivní");
      } else {
        record.emplace_back(rokZaniku);
      }
      record.emplace_back(Utilities::defaultForEmpty(CINNOSTI_DLE_NACE_field->getAsString(), notAvailable));
      writer.writeRecord(record);
    }
  }

  logger.endTime();
  logger.printElapsedTime();

  // join na seznam pozadovanych ico jako prvni, pak zjistit pravni formu z ciselniku FORMA, velikostni kategorii z
  // ciselniku KATPO slozit adresu z polozek, dohledat kraj z ciselniku OKRESLAU, zmensit datum na orpavdu pouze datum,
  // pridat k datum sloupce pouze pro rok zjisit OBEC_ICOB podle ICZUJ a prejmenovat sloupce

  // DataWriters::CsvWriter writer{"/home/petr/Desktop/test.csv"};
  // writer.setAddQuotes(true);
  // writer.writeDataSet(*db.execSimpleQuery(queryCisleniky, false, "result")->dataSet);
  /*

  const auto query = "SELECT res.ICO, "
                     "res.Stav, "
                     "res.Nazev, "
                     "res.Statisticka_pravni_forma, "
                     "res.Datum_vzniku, "
                     "res.Datum_zaniku, "
                     "res.Adresa, "
                     "res.\"Kod_stat._pravni_formy\", "
                     "res.\"Institucionalni_sektor\", "
                     "res.\"Cinnosti_dle_CZ-NACE\", "
                     "vel_kod.nazev FROM res JOIN vel_kod on res.\"Velikostni_kategorie\" = vel_kod.kod;";

  auto result = db.execSimpleQuery(query, false, "resKat");

  db.addTable(result->dataSet->toDataSet());
  db.removeTable("res");
  db.removeTable("vel_kod");

  DataWriters::CsvWriter writer{"/home/petr/Desktop/out.csv"};
  auto r = db.tableByName("resKat")->dataSet;
  writer.writeHeader(r->getFieldNames());
  std::vector<std::string> row;
  auto fields = r->getFields();
  writer.setAddQuotes(true);
  while (r->next()) {
    std::transform(fields.begin(), fields.end(), std::back_inserter(row),
                   [](auto &field) { return field->getAsString(); });
    writer.writeRecord(row);
    row.clear();
  }*/

  return 0;
}