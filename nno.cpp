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
Logger logger{std::cout};


const auto notAvailable = "není k dispozici"s;
std::shared_ptr<DataSets::MemoryDataSet> prepareNACE(const std::shared_ptr<DataSets::MemoryDataSet> &res) {
  DataBase::MemoryDataBase db("db");
  db.addTable(res);

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
  logger.setDefaultPrintTime(true);
  logger << status{} << "Starting\n";

  DataBase::MemoryDataBase db("db");

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

  auto resTable = db.addTable(std::make_shared<DataSets::MemoryDataSet>("resFiltered"));
  {
    logger << debug{} << "Starting NNO selection\n";
    const std::vector resColumnTypes{ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
                                     ValueType::String,  ValueType::String, ValueType::String, ValueType::String};
    auto resDs = createDataSetFromFile(
        "r", FileSettings::Csv("/home/petr/Desktop/muni/res/all/RES3A2020M02.csv", ';', true, CharSet::CP1250),
        resColumnTypes);
    const auto resColumnNames = resDs->getFieldNames();
    resTable->dataSet->openEmpty(resColumnNames, resColumnTypes);

    DataProviders::CsvReader nnoKodyReader{"/home/petr/Desktop/muni/pravni_formy_nno.csv"};
    std::vector<std::string> nnoKody;
    std::transform(nnoKodyReader.begin(), nnoKodyReader.end(), std::back_inserter(nnoKody), [] (const auto &row) {
      return row[0];
    });

    auto srcFields = resDs->getFields();
    auto srcPravniFormaKodField = resDs->fieldByName("FORMA");
    auto dstFields = resTable->dataSet->getFields();
    while (resDs->next()) {
      if (isIn(srcPravniFormaKodField->getAsString(), nnoKody)) {
        resTable->dataSet->append();
        for (auto i : MakeRange::range(srcFields.size())) {
          dstFields[i]->setAsString(srcFields[i]->getAsString());
        }
      }
    }
    logger << debug{} << fmt::format("NNO selection done, row count: {}", resTable->dataSet->getCurrentRecord());
    resTable->dataSet->resetBegin();
  }

  db.addTable(prepareNACE(resTable->dataSet));
  logger << status{} << "Prepared NACE\n";

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

  {
    std::vector<std::string> okresniMesta{};
    {
      DataProviders::CsvReader csvReader("/home/petr/Desktop/muni/res/okresni_mesta.csv");
      std::transform(csvReader.begin(), csvReader.end(), std::back_inserter(okresniMesta), [](const auto &row) {
        return row[0];
      });
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
  return 0;
}