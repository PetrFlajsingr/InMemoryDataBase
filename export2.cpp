//
// Created by petr on 10/7/19.
//

#include "Combiner.h"
#include <CsvStreamReader.h>
#include <CsvWriter.h>
#include <Logger.h>
#include <MemoryDataBase.h>
#include <MemoryDataSet.h>
#include <XlntWriter.h>
#include <XlsxIOReader.h>
#include <XlsxReader.h>
#include <memory>

using namespace std::string_literals;

auto &logger = Logger<true>::GetInstance();

struct FileSettings {
  enum Type { csv, xlsx, xls };
  Type type;
  std::string pathToFile;
  char delimiter;
  std::string sheet = "";

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

  static FileSettings Csv(const std::string &path, char delimiter = ',') {
    FileSettings result;
    result.type = csv;
    result.pathToFile = path;
    result.delimiter = delimiter;
    return result;
  }
};

std::shared_ptr<DataSets::MemoryDataSet> createDataSetFromFile(const std::string &name,
                                                               const FileSettings &fileSettings,
                                                               const std::vector<ValueType> &valueTypes) {
  std::unique_ptr<DataProviders::BaseDataProvider> provider;
  switch (fileSettings.type) {
  case FileSettings::csv:
    provider = std::make_unique<DataProviders::CsvStreamReader>(fileSettings.pathToFile, fileSettings.delimiter);
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

const auto folder = "/home/petr/Desktop/export2/"s;

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
void combine(std::vector<std::shared_ptr<DataSets::BaseDataSet>> dataSets, std::vector<TableColumn> joins,
             std::string dest) {
  std::vector<DSFieldCnt> dss;
  gsl::index fieldOffset = 0;
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
  }
  int rowLength = fieldOffset;
  auto mainDS = dss[0];
  auto mainField = dynamic_cast<DataSets::IntegerField *>(dss[0].field);

  std::vector<std::string> row;

  DataWriters::XlntWriter writer(dest);
  for (auto &ds : dss) {
    std::transform(ds.fields.begin(), ds.fields.end(), std::back_inserter(row),
                   [](auto &field) { return field->getName().data(); });
  }

  writer.writeHeader(row);

  dss = std::vector<DSFieldCnt>{dss.begin() + 1, dss.end()};

  while (mainDS.ds->next()) {
    for (auto &ds : dss) {
      ds.ds->setCurrentRecord(ds.pos);
      while (ds.ds->next()) {
        if (mainField->getAsInteger() == ds.field->getAsInteger()) {
          row.clear();
          std::transform(mainDS.fields.begin(), mainDS.fields.end(), std::back_inserter(row),
                         [](auto &field) { return field->getAsString(); });
          int fill = ds.fieldOffset - mainDS.fields.size();
          for (int i = 0; i < fill; ++i) {
            row.emplace_back("(NULL)");
          }
          std::transform(ds.fields.begin(), ds.fields.end(), std::back_inserter(row),
                         [](auto &field) { return field->getAsString(); });

          fill = rowLength - row.size();
          for (int i = 0; i < fill; ++i) {
            row.emplace_back("(NULL)");
          }

          writer.writeRecord(row);
        } else if (mainField->getAsInteger() < ds.field->getAsInteger()) {
          ds.pos = ds.ds->getCurrentRecord() - 1;
          break;
        }
      }
    }
  }
}

std::shared_ptr<DataSets::MemoryDataSet> transformDotace(const std::shared_ptr<DataSets::BaseDataSet>& dotaceSub) {
  auto dotaceRequired = std::make_shared<DataSets::MemoryDataSet>("dotace2");
  dotaceRequired->openEmpty(
      {"DOTACE_POSKYTOVATEL", "DOTACE_CASTKA_UVOLNENA", "DOTACE_CASTKA_CERPANA", "DOTACE_ROK",
       "DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU", "DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA",
       "DOTACE_REGISTRACNI_CISLO_PROJEKTU", "DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU",
       "DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU", "DOTACE_OPERACNI_PROGRAM_PRIJEMCE",
       "DOTACE_OPERACNI_PROGRAM_DATUM__SKUTECNE_DATUM_ZAHAJENI_FYZICKE_REALIZACE_PROJEKTU",
       "DOTACE_OPERACNI_PROGRAM_DATUM_REALIZACE_SKUTECNE_DATUM_UKONCENI_FYZICKE_REALIZACE_PROJEKTU",
       "DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_KOD", "DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_NAZEV",
       "DOTACE_OPERACNI_PROGRAM_MISTO_REALIZACE_NUTS3_NAZEV", "DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_CASTKA",
       "DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_EU",
       "DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR"},
      {ValueType::String, ValueType::Currency, ValueType::Currency, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::Integer, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::Currency, ValueType::Currency,
       ValueType::Currency});

  dotaceSub->resetBegin();
  auto DOTACE_POSKYTOVATEL = dotaceSub->fieldByName("Poskytovatel");
  auto DOTACE_CASTKA_UVOLNENA1 =
      dynamic_cast<DataSets::CurrencyField*>(dotaceSub->fieldByName("Celkové způsobilé výdaje přidělené na operaci (příspěvek Unie) CZK"));
  auto DOTACE_CASTKA_UVOLNENA2 = dynamic_cast<DataSets::CurrencyField*>(dotaceSub->fieldByName("Celkové způsobilé výdaje přidělené na operaci (národní soukromé zdroje / soukromé zdroje) CZK"));

  auto DOTACE_CASTKA_CERPANA1 = dynamic_cast<DataSets::CurrencyField*>(dotaceSub->fieldByName("Finanční prostředky vyúčtované v žádostech o platbu (příspěvek Unie, CZK)"));
  auto DOTACE_CASTKA_CERPANA2 = dynamic_cast<DataSets::CurrencyField*>(dotaceSub->fieldByName("Finanční prostředky vyúčtované v žádostech o platbu (národní veřejné zdroje, CZK)"));

  auto DOTACE_ROK = dotaceSub->fieldByName("Datum zahájení fyzické operace");

  auto DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU = dotaceSub->fieldByName("Program");
  auto DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA = dotaceSub->fieldByName("Název prioritní osy / priority Unie");
  auto DOTACE_REGISTRACNI_CISLO_PROJEKTU = dotaceSub->fieldByName("Registrační číslo projektu/operace");
  auto DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU = dotaceSub->fieldByName("Název projektu / Název operace");
  auto DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU = dotaceSub->fieldByName("Shrnutí operace");
  auto DOTACE_OPERACNI_PROGRAM_PRIJEMCE = dotaceSub->fieldByName("IC");
  auto DOTACE_OPERACNI_PROGRAM_DATUM__SKUTECNE_DATUM_ZAHAJENI_FYZICKE_REALIZACE_PROJEKTU = dotaceSub->fieldByName("Skutečné datum ukončení fyzické realizace operace");
  auto DOTACE_OPERACNI_PROGRAM_DATUM_REALIZACE_SKUTECNE_DATUM_UKONCENI_FYZICKE_REALIZACE_PROJEKTU = dotaceSub->fieldByName("Skutečné datum ukončení fyzické realizace operace");
  auto DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_KOD = dotaceSub->fieldByName("Oblast intervence - kód");
  auto DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_NAZEV = dotaceSub->fieldByName("Oblast intervence - název");
  auto DOTACE_OPERACNI_PROGRAM_MISTO_REALIZACE_NUTS3_NAZEV = dotaceSub->fieldByName("Název NUTS 3");
  auto DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_CASTKA = dynamic_cast<DataSets::CurrencyField*>(dotaceSub->fieldByName("Celkové způsobilé výdaje přidělené na operaci (národní soukromé zdroje / soukromé zdroje) CZK"));
  auto DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_EU = dynamic_cast<DataSets::CurrencyField*>(dotaceSub->fieldByName("Celkové způsobilé výdaje přidělené na operaci (příspěvek Unie) CZK"));
  auto DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR = dynamic_cast<DataSets::CurrencyField*>(dotaceSub->fieldByName("Celkové způsobilé výdaje přidělené na operaci (veřejné zdroje ČR) CZK"));
  while(dotaceSub->next()) {
    dotaceRequired->append();
    dotaceRequired->fieldByIndex(0)->setAsString(DOTACE_POSKYTOVATEL->getAsString());
    reinterpret_cast<DataSets::CurrencyField*>(dotaceRequired->fieldByIndex(1))->setAsCurrency(DOTACE_CASTKA_UVOLNENA1->getAsCurrency() + DOTACE_CASTKA_UVOLNENA2->getAsCurrency());
    reinterpret_cast<DataSets::CurrencyField*>(dotaceRequired->fieldByIndex(2))->setAsCurrency(DOTACE_CASTKA_CERPANA1->getAsCurrency() + DOTACE_CASTKA_CERPANA2->getAsCurrency());
    dotaceRequired->fieldByIndex(3)->setAsString(DOTACE_ROK->getAsString());
    dotaceRequired->fieldByIndex(4)->setAsString(DOTACE_OPERACNI_PROGRAM_NAZEV_PROGRAMU->getAsString());
    dotaceRequired->fieldByIndex(5)->setAsString(DOTACE_OPERACNI_PROGRAM_PRIORITNI_OSA->getAsString());
    dotaceRequired->fieldByIndex(6)->setAsString(DOTACE_REGISTRACNI_CISLO_PROJEKTU->getAsString());
    dotaceRequired->fieldByIndex(7)->setAsString(DOTACE_OPERACNI_PROGRAM_NAZEV_PROJEKTU->getAsString());
    dotaceRequired->fieldByIndex(8)->setAsString(DOTACE_OPERACNI_PROGRAM_ANOTACE_PROJEKTU->getAsString());
    dotaceRequired->fieldByIndex(9)->setAsString(DOTACE_OPERACNI_PROGRAM_PRIJEMCE->getAsString());
    dotaceRequired->fieldByIndex(10)->setAsString(DOTACE_OPERACNI_PROGRAM_DATUM__SKUTECNE_DATUM_ZAHAJENI_FYZICKE_REALIZACE_PROJEKTU->getAsString());
    dotaceRequired->fieldByIndex(11)->setAsString(DOTACE_OPERACNI_PROGRAM_DATUM_REALIZACE_SKUTECNE_DATUM_UKONCENI_FYZICKE_REALIZACE_PROJEKTU->getAsString());
    dotaceRequired->fieldByIndex(12)->setAsString(DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_KOD->getAsString());
    dotaceRequired->fieldByIndex(13)->setAsString(DOTACE_OPERACNI_PROGRAM_OBLAST_INTERVENCE_NAZEV->getAsString());
    dotaceRequired->fieldByIndex(14)->setAsString(DOTACE_OPERACNI_PROGRAM_MISTO_REALIZACE_NUTS3_NAZEV->getAsString());
    reinterpret_cast<DataSets::CurrencyField*>(dotaceRequired->fieldByIndex(15))->setAsCurrency(DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_CASTKA->getAsCurrency());
    reinterpret_cast<DataSets::CurrencyField*>(dotaceRequired->fieldByIndex(16))->setAsCurrency(DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_EU->getAsCurrency());
    reinterpret_cast<DataSets::CurrencyField*>(dotaceRequired->fieldByIndex(17))->setAsCurrency(DOTACE_OPERACNI_PROGRAM_SPOLUFINANCOVANI_PRISPEVEK_CR->getAsCurrency());
  }
return dotaceRequired;
}

template <typename Container>
void print(Container container) {
  std::cout << "{";
  for (auto &value : container) {
    std::cout << "\n"  << value << ",";
  }
  std::cout << "}" << std::endl;
}

int main() {
  DataBase::MemoryDataBase db("db");

  auto subjekty = createDataSetFromFile("subjekty", FileSettings::Xlsx(folder + "NNO.xlsx"),
                                        {ValueType::Integer, ValueType::Integer, ValueType::String, ValueType::String,
                                         ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                         ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                         ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                         ValueType::String});
  db.addTable(subjekty);
  auto result = db.execSimpleQuery("select subjekty.ICOnum as ico, subjekty.NAZEV from subjekty;", true, "sub");

  db.execSimpleQuery("select subjekty.* from subjekty;", true, "sub");

  auto VZds = createDataSetFromFile(
      "VZ", FileSettings::Xlsx(folder + "vz.xlsx", "VZ"),
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
      "castiVZ", FileSettings::Xlsx(folder + "vz.xlsx", "Části VZ"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String});
  auto dodavatelVZ = createDataSetFromFile(
      "dodVZ", FileSettings::Xlsx(folder + "vz.xlsx", "Dodavatelé"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String});

  db.addTable(VZds);
  db.addTable(castiVZds);
  db.addTable(dodavatelVZ);


  const std::string query = "select VZ.EvidencniCisloVZnaVVZ, "
                            "VZ.LimitVZ, "
                            "VZ.OdhadovanaHodnotaVZbezDPH, "
                            "castiVZ.PredpokladanaCelkovaHodnotaCastiVZ, "
                            "VZ.ZadavatelICO, "
                            "VZ.DatumUverejneni, "
                            "VZ.NazevVZ, "
                            "VZ.DruhVZ, "
                            "VZ.StrucnyPopisVZ, "
                            "VZ.ZadavatelUredniNazev, "
                            "castiVZ.ID_CastiVZ, "
                            "castiVZ.NazevCastiVZ, dodVZ.DodavatelICO from VZ "
                            "join castiVZ on VZ.ID_Zakazky = castiVZ.ID_Zakazky "
                            "join dodVZ on VZ.ID_Zakazky = dodVZ.ID_Zakazky "
                            "join subjekty on dodVZ.DodavatelICO = subjekty.ICOnum;";

  db.addTable(db.execSimpleQuery(query, false, "vz")->dataSet->toDataSet());
  logger.log<LogLevel::Info>("Query vz done");
  db.tableByName("vz")->dataSet->resetEnd();
  logger.log<LogLevel::Debug, true>(db.tableByName("vz")->dataSet->getCurrentRecord());

  auto cedr = createDataSetFromFile(
      "cedr", FileSettings::Xlsx(folder + "cedr.xlsx"),
      {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::Integer,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String});
  db.addTable(cedr);
  db.execSimpleQuery("select cedr.dotacePoskytovatelNazev, "
                     "cedr.castkaUvolnena, "
                     "cedr.castkaCerpana, "
                     "cedr.rozpoctoveObdobi, "
                     "cedr.operacniProgramNazev, "
                     "cedr.dotaceTitulNazev, "
                     "cedr.projektIdnetifikator, "
                     "cedr.ICOnum from cedr;",
                     true, "cedr2");
  logger.log<LogLevel::Info>("Query cedr done");

  auto dotaceDS = createDataSetFromFile(
      "dotace", FileSettings::Xlsx(folder + "dotaceeu.xlsx"),
      {ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
       ValueType::String, ValueType::String,  ValueType::String, ValueType::String, ValueType::String,
       ValueType::Currency, ValueType::Currency,  ValueType::Currency, ValueType::Currency, ValueType::Currency,
       ValueType::Currency, ValueType::Currency,  ValueType::Currency, ValueType::String});
  db.addTable(dotaceDS);

  auto dotaceDoplneniDS = createDataSetFromFile(
      "dotace_dopl", FileSettings::Xlsx(folder + "NNO_doplneni.xlsx", "dotaceeu_poskytovatel"),
      {ValueType::String, ValueType::String});
  db.addTable(dotaceDoplneniDS);
  auto dotaceSub = db.execSimpleQuery("select dotace.*, subjekty.ICOnum, dotace_dopl.Poskytovatel from dotace "
                                      "join subjekty on dotace.IC = subjekty.ICOnum "
                                      "left join dotace_dopl on dotace.Program = dotace_dopl.Program;",
                                      false, "dotaceSub");
  logger.log<LogLevel::Info>("Query dotace2done");

  auto dotace2 = transformDotace(dotaceSub->dataSet);
  db.addTable(dotace2);
  db.execSimpleQuery("select dotace2.* from dotace2;", true, "dotace2");
  db.execSimpleQuery("select vz.* from vz join subjekty on vz.DodavatelICO = subjekty.ICOnum;", true, "vz2");
  logger.log<LogLevel::Info>("Query vz2 done");

  auto dssub = db.viewByName("sub");
  auto dsvz2 = db.viewByName("vz2");
  auto dscedr2 = db.viewByName("cedr2");
  auto dsdotace2 = db.viewByName("dotace2");
  dssub->dataSet->resetBegin();

  combine({db.viewByName("sub")->dataSet, db.viewByName("vz2")->dataSet, db.viewByName("cedr2")->dataSet,
           db.viewByName("dotace2")->dataSet},
          {{"sub", "ICOnum"}, {"vz2", "DodavatelICO"}, {"cedr2", "ICOnum"}, {"dotace2", "DOTACE_OPERACNI_PROGRAM_PRIJEMCE"}},
          folder + "export2.xlsx");
}