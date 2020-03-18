//
// Created by petr on 3/14/20.
//
#include "LoadingUtils.h"
#include "fmt/format.h"
#include <CsvWriter.h>
#include <MemoryDataBase.h>
#include <io/print.h>
#include <string>

using namespace std::string_literals;

const auto csvPath = "/home/petr/Desktop/muni/"s;
const auto outPath = csvPath + "out/";
const auto rpssPath = csvPath + "rpss/Prehled_socialnich_sluzeb_CR_2.3.2020.xlsx";
const auto resPath = csvPath + "res/res_full.csv";
const auto notAvailable = "není k dispozici"s;
const auto justicePath = csvPath + "or.justice/justice.csv";

int main() {
  // data jsou zurvena
  DataBase::MemoryDataBase db{"db"};
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
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String,
      ValueType::String, ValueType::String,  ValueType::String,
  };
  db.addTable(createDataSetFromFile("rpss", FileSettings::Xlsx(rpssPath, "List1"), rpssTypes));

  const auto queryZarizeni = R"(
SELECT
rpss."Název zařízení" as NAZEV_ZARIZENI,
rpss."Druh sociální služby",
rpss."Poskytována od",
rpss."cílová skupina",
rpss."Věková kategorie",
rpss."Ambul. forma",
rpss."Pobyt. forma",
rpss."Teren. forma",
rpss."Adresa textově - Z",
rpss."Kontakty služba - Telefon",
rpss."Kontakty služba - Web",
rpss."Kraj - Z",
rpss."Okres - Z",
rpss."Obec - Z",
rpss."Název",
rpss."IČ",
rpss."Adresa textově - P"
FROM rpss;
)";

  db.execSimpleQuery(queryZarizeni, false, "rpss");
}