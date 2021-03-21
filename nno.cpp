//
// Created by petr on 2/11/20.
//

#include "LoadingUtils.h"
#include "io/print.h"
#include "types/Range.h"
#include "various/isin.h"
#include <CsvReader.h>
#include <CsvWriter.h>
#include <MemoryDataBase.h>
#include <XlsxIOReader.h>
#include <include/fmt/format.h>

using namespace std::string_literals;
using namespace LoggerStreamModifiers;
Logger loggerNNO{std::cout};

const auto notAvailable = "není k dispozici"s;
const auto csvPath = "/home/petr/Desktop/muni/"s;
// const auto nrzpsPath = csvPath + "nrzps/narodni-registr-poskytovatelu-zdravotnich-sluzeb.csv";

std::shared_ptr<DataSets::MemoryDataSet> prepareNACE(const std::shared_ptr<DataSets::MemoryDataSet> &res) {
  DataBase::MemoryDataBase db("db");
  db.addTable(res);

  db.addTable(createDataSetFromFile(
      "res_nace", FileSettings::Csv("/home/petr/Desktop/muni/res/all/RES3B2020M12.csv", ';', true, CharSet::CP1250),
      {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String}));
  db.addTable(createDataSetFromFile(
      "nace", FileSettings::Csv(csvPath + "res/all/DM_ERES_ERES_RES_U_CIS_80004.csv", ';', true, CharSet::CP1250),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String}));
  auto ordered = db.execSimpleQuery(R"(SELECT res_nace.ICO, res_nace.HODN, nace.TEXTZ
FROM res_nace
JOIN resFiltered ON res_nace.ICO = resFiltered.ICO
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
  loggerNNO.setDefaultPrintTime(true);
  loggerNNO << status{} << "Starting\n";

  DataBase::MemoryDataBase db("db");
  // db.addTable(createDataSetFromFile(
  //    "nrzps", FileSettings::Csv(nrzpsPath),
  //    {ValueType::String, ValueType::String, ValueType::String, ValueType::String,  ValueType::String,
  //     ValueType::String, ValueType::String, ValueType::String, ValueType::String,  ValueType::String,
  //     ValueType::String, ValueType::String, ValueType::String, ValueType::String,  ValueType::String,
  //     ValueType::String, ValueType::String, ValueType::String, ValueType::Integer, ValueType::String,
  //     ValueType::String, ValueType::String, ValueType::String, ValueType::String,  ValueType::String,
  //     ValueType::String, ValueType::String, ValueType::String, ValueType::String,  ValueType::String,
  //     ValueType::String, ValueType::String, ValueType::String}));

  db.addTable(createDataSetFromFile(
      "forma", FileSettings::Csv(csvPath + "res/all/DM_ERES_ERES_RES_U_CIS_149.csv", ';', true, CharSet::CP1250),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String}));

  db.addTable(createDataSetFromFile(
      "katpo", FileSettings::Csv(csvPath + "res/all/DM_ERES_ERES_RES_U_CIS_579.csv", ';', true, CharSet::CP1250),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String}));

  db.addTable(createDataSetFromFile(
      "okreslau", FileSettings::Csv(csvPath + "res/all/DM_ERES_ERES_RES_U_CIS_109.csv", ';', true, CharSet::CP1250),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String}));

  db.addTable(createDataSetFromFile("rso", FileSettings::Csv(csvPath + "res/rso.csv", ','),
                                    {ValueType::String, ValueType::String, ValueType::String, ValueType::String}));
  auto resDs =
      db
          .addTable(createDataSetFromFile(
              "resFiltered", FileSettings::Csv(csvPath + "res/all/RES3A2020M12.csv", ';', true, CharSet::CP1250),
              {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
               ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
               ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
               ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
               ValueType::String,  ValueType::String, ValueType::String, ValueType::String}))
          ->dataSet;

  db.addTable(prepareNACE(resDs));
  loggerNNO << status{} << "Prepared NACE\n";

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
resFiltered.DDATZAN AS DATUM_LIKVIDACE,
resFiltered.PSC,
resFiltered.COBCE_TEXT,
resFiltered.ULICE_TEXT,
resFiltered.CDOM,
resFiltered.COR,
resFiltered.FORMA,
nace.CINNOSTI_DLE_NACE,
nace.KOD_NACE
FROM resFiltered
LEFT JOIN nace on resFiltered.ICO = nace.ICO
LEFT JOIN forma ON resFiltered.FORMA = forma.KODZAZ
LEFT JOIN katpo ON resFiltered.KATPO = katpo.KODZAZ
LEFT JOIN okreslau ON resFiltered.OKRESLAU = okreslau.KODZAZ
LEFT JOIN rso on resFiltered.ICZUJ = rso.KOD_ZUJ
ORDER BY resFiltered.ICO ASC
;)sql";

  {
    std::vector<std::string> nnoKody;
    {
      loggerNNO << debug{} << "Starting NNO selection\n";
      const std::vector resColumnTypes{ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                       ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
                                       ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
                                       ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
                                       ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
                                       ValueType::String,  ValueType::String, ValueType::String, ValueType::String};
      DataProviders::CsvReader nnoKodyReader{csvPath + "pravni_formy_nno.csv"};
      std::transform(nnoKodyReader.begin(), nnoKodyReader.end(), std::back_inserter(nnoKody),
                     [](const auto &row) { return row[0]; });
    }

    std::vector<std::string> okresniMesta{};
    {
      DataProviders::CsvReader csvReader(csvPath + "res/okresni_mesta.csv");
      std::transform(csvReader.begin(), csvReader.end(), std::back_inserter(okresniMesta),
                     [](const auto &row) { return row[0]; });
    }
    auto result = db.execSimpleQuery(queryCisleniky, false, "result")->dataSet;
    auto ICO_field = dynamic_cast<DataSets::IntegerField *>(result->fieldByName("ICO")); // ICOnum
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
    auto DATUM_LIKVIDACE_field =
        result->fieldByName("DATUM_LIKVIDACE"); // potreba predelat na normalni datum, zkratit a nahradit - /
    // ROK_LIKVIDACE
    // INSTITUCE_V_LIKVIDACI - pokud je datum zaniku prazdne tak aktivní, jinak rok zaniku
    auto PSC_field = result->fieldByName("PSC");
    auto COBCE_TEXT_field = result->fieldByName("COBCE_TEXT");
    auto ULICE_TEXT_field = result->fieldByName("ULICE_TEXT");
    auto CDOM_field = result->fieldByName("CDOM");
    auto COR_field = result->fieldByName("COR");
    auto CINNOSTI_DLE_NACE_field = result->fieldByName("CINNOSTI_DLE_NACE");
    auto KOD_NACE_field = result->fieldByName("KOD_NACE");

    auto FORMA_field = result->fieldByName("FORMA");

    DataWriters::CsvWriter writer{csvPath + "res/res.csv"};
    writer.setAddQuotes(true);

    DataWriters::CsvWriter writerFull{csvPath + "res/res_full.csv"};
    writerFull.setAddQuotes(true);

    result->resetBegin();
    writer.writeHeader({"ICO_number", "ICO", "NAZEV", "PRAVNI_FORMA", "VELIKOSTNI_KATEGORIE", "ADRESA", "KRAJ", "OKRES",
                        "OBEC", "OBEC_OKRES", "OBEC_ICOB", "DATUM_VZNIKU", "ROK_VZNIKU", "DATUM_LIKVIDACE",
                        "ROK_LIKVIDACE", "INSTITUCE_V_LIKVIDACI", "NACE_cinnost", "NACE_kod", "FORMA"});
    writerFull.writeHeader({"ICO_number", "ICO", "NAZEV", "PRAVNI_FORMA", "VELIKOSTNI_KATEGORIE", "ADRESA", "KRAJ",
                            "OKRES", "OBEC", "OBEC_OKRES", "OBEC_ICOB", "DATUM_VZNIKU", "ROK_VZNIKU", "DATUM_LIKVIDACE",
                            "ROK_LIKVIDACE", "INSTITUCE_V_LIKVIDACI", "NACE_cinnost", "NACE_kod", "FORMA"});
    std::vector<std::string> record;
    while (result->next()) {
      record.clear();
      record.emplace_back(ICO_field->getAsString());
      record.emplace_back(fmt::format("{:08d}", ICO_field->getAsInteger()));
      record.emplace_back(
          Utilities::defaultForEmpty(Utilities::replaceAll(NAZEV_field->getAsString(), "&#34^", "\""), notAvailable));
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
      record.emplace_back(Utilities::defaultForEmpty(obec, notAvailable));
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
      auto datumZaniku = DATUM_LIKVIDACE_field->getAsString();
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
      record.emplace_back(Utilities::defaultForEmpty(KOD_NACE_field->getAsString(), notAvailable));
      record.emplace_back(FORMA_field->getAsString());
      if (isIn(FORMA_field->getAsString(), nnoKody)) {
        writer.writeRecord(record);
      }
      writerFull.writeRecord(record);
    }
  }
  loggerNNO << debug{} << "Done";
  return 0;
}