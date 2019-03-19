//
// Created by Petr Flajsingr on 2019-03-18.
//

#include <CsvReader.h>
#include <MemoryDataSet.h>
#include <MemoryDataBase.h>
#include <XlsReader.h>
#include <XlsxWriter.h>
#include <CsvWriter.h>
#include <XlntWriter.h>
#include <boost/locale.hpp>

int main() {
  //auto nnoProvider = DataProviders::CsvReader(
  //    "/Users/petr/Desktop/csvs/NNO_subjekty6ver2.4.csv",
  //    ";");
  auto nnoProvider =
      DataProviders::XlsReader("/Users/petr/Desktop/NNO_subjekty6ver2.4.xlsx");
  auto nnoDs = std::make_shared<DataSets::MemoryDataSet>("nno");
  //nnoDs->open(nnoProvider,
  //            {ValueType::Integer, ValueType::Integer, ValueType::Integer,
  //             ValueType::String, ValueType::String, ValueType::String,
  //             ValueType::String});
  nnoDs->open(nnoProvider,
              {ValueType::Integer, ValueType::Integer, ValueType::Integer,
               ValueType::Integer, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String, ValueType::String, ValueType::String,
               ValueType::String});

  auto zdravProvider =
      DataProviders::CsvReader("/Users/petr/Desktop/csvs/export-2019-02.csv",
                               ";");
  auto zdravDs = std::make_shared<DataSets::MemoryDataSet>("zdrav");
  zdravDs->open(zdravProvider,
                {ValueType::String, ValueType::String, ValueType::String,
                 ValueType::String, ValueType::String, ValueType::String,
                 ValueType::String, ValueType::String, ValueType::String,
                 ValueType::String, ValueType::String, ValueType::String,
                 ValueType::String, ValueType::String, ValueType::String,
                 ValueType::String, ValueType::String, ValueType::String,
                 ValueType::String, ValueType::String, ValueType::String,
                 ValueType::Integer, ValueType::String, ValueType::String,
                 ValueType::String, ValueType::String, ValueType::String,
                 ValueType::String, ValueType::String, ValueType::String,
                 ValueType::String, ValueType::String, ValueType::String,
                 ValueType::String, ValueType::String, ValueType::String,
                 ValueType::String, ValueType::String});

  DataBase::MemoryDataBase db("main");
  db.addTable(nnoDs);
  db.addTable(zdravDs);

  auto joinRes = db.execSimpleQuery("select zdrav.*, nno.* from nno join zdrav "
                                    "on nno.ICO_num = zdrav.Ico;",
                                    false, "joinRes");

  auto nrzps_nazevZarField = joinRes->dataSet->fieldByName("NazevCely");
  auto nrzps_druhZarField = joinRes->dataSet->fieldByName("DruhZarizeni");
  auto nrzps_obecZarField = joinRes->dataSet->fieldByName("Obec");
  auto nrzps_pscZarField = joinRes->dataSet->fieldByName("Psc");
  auto nrzps_uliceZarField = joinRes->dataSet->fieldByName("Ulice");
  auto nrzps_cisloDomZarField =
      joinRes->dataSet->fieldByName("CisloDomovniOrientacni");
  auto
      nrzps_nazevPoskField = joinRes->dataSet->fieldByName("PoskytovatelNazev");
  auto nrzps_pravFormaField = joinRes->dataSet->fieldByName("PravniFormaKod");
  auto nrzps_krajPoskField = joinRes->dataSet->fieldByName("KrajSidlo");
  auto nrzps_okresPoskField = joinRes->dataSet->fieldByName("OkresSidlo");
  auto nrzps_pscPoskField = joinRes->dataSet->fieldByName("PscSidlo");
  auto nrzps_obecPoskField = joinRes->dataSet->fieldByName("ObecSidlo");
  auto nrzps_ulicePoskyField = joinRes->dataSet->fieldByName("UliceSidlo");
  auto nrzps_cisloDomPoskField =
      joinRes->dataSet->fieldByName("CisloDomovniOrientacniSidlo");
  auto nrzps_oborPeceField = joinRes->dataSet->fieldByName("OborPece");
  auto nrzps_formaPeceField = joinRes->dataSet->fieldByName("FormaPece");
  auto nrzps_gpsField = joinRes->dataSet->fieldByName("GPS");
  auto nno_icField =
      dynamic_cast<DataSets::IntegerField *>(joinRes->dataSet->fieldByName(
          "ICO_num"));
  auto nno_yField = joinRes->dataSet->fieldByName("Y");
  auto nno_xField = joinRes->dataSet->fieldByName("X");
  auto nno_obecIdField = joinRes->dataSet->fieldByName("ICOB_text");

  auto joinDs = joinRes->dataSet->toDataSet();
  db.addTable(joinDs);
  //db.removeTable("nno");
  //db.removeTable("zdrav");

  auto countRes = db.execAggregateQuery(
      "select joinRes.ICO_num, COUNT(joinRes.PoskytovatelNazev) "
      "from joinRes group by joinRes.ICO_num;",
      "count");

  joinRes->dataSet->resetBegin();
  countRes->dataSet->resetBegin();
  auto result = DataSets::MemoryDataSet("result");
  result.openEmpty({"IC_POSKYTOVATEL",
                    "NAZEV_POSKYTOVATELE",
                    "PRAVNI_FORMA",
                    "ULICE",
                    "CISLO_DOMOVNI_ORIENTACNI",
                    "KRAJ",
                    "OKRES",
                    "OBEC",
                    "OBEC_ID",
                    "PSC",
                    "POCET_ZDRAVOTNICKYCH_ZARIZENI",
                    "X",
                    "Y",
                    "NAZEV_ZARIZENI",
                    "OBEC_ZARIZENI",
                    "ULICE_ZARIZENI",
                    "CISLO_DOMOVNI_ORIENTACNI_ZARIZENI",
                    "PSC_ZARIZENI",
                    "OBOR_PECE",
                    "FORMA_PECE",
                    "ORGANIZACE/ZARIZENI"
                   },
                   {ValueType::Integer,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String,
                    ValueType::String});

  auto res_icField = dynamic_cast<DataSets::IntegerField *>(result.fieldByName(
      "IC_POSKYTOVATEL"));
  auto res_nazevPoskField = result.fieldByName("NAZEV_POSKYTOVATELE");
  auto res_pravFormField = result.fieldByName("PRAVNI_FORMA");
  auto res_ulicePoskField = result.fieldByName("ULICE");
  auto res_cisloDomPoskyField = result.fieldByName("CISLO_DOMOVNI_ORIENTACNI");
  auto res_krajPoskField = result.fieldByName("KRAJ");
  auto res_okresPoskField = result.fieldByName("OKRES");
  auto res_obecPoskField = result.fieldByName("OBEC");
  auto res_obecIdPoskField = result.fieldByName("OBEC_ID");
  auto res_pscPoskField = result.fieldByName("PSC");
  auto res_pocetZarField = result.fieldByName("POCET_ZDRAVOTNICKYCH_ZARIZENI");
  auto res_xField = result.fieldByName("X");
  auto res_yField = result.fieldByName("Y");
  auto res_nazevZarField = result.fieldByName("NAZEV_ZARIZENI");
  auto res_obecZarField = result.fieldByName("OBEC_ZARIZENI");
  auto res_uliceZarField = result.fieldByName("ULICE_ZARIZENI");
  auto res_cisloDomZarField =
      result.fieldByName("CISLO_DOMOVNI_ORIENTACNI_ZARIZENI");
  auto res_oborPeceField = result.fieldByName("OBOR_PECE");
  auto res_formaPeceField = result.fieldByName("FORMA_PECE");
  auto res_orgZarField = result.fieldByName("ORGANIZACE/ZARIZENI");
  auto res_pscZarField = result.fieldByName("PSC_ZARIZENI");

  int lastIco = -1;
  auto countField =
      dynamic_cast<DataSets::IntegerField *>(countRes->dataSet->fieldByName(
          "PoskytovatelNazev"));

  const std::string nilStr = "Null";
  countRes->dataSet->next();
  bool setPoskytovatel = false;
  while (joinRes->dataSet->next()) {
    yay_goto:
    result.append();
    setPoskytovatel = false;
    if (nno_icField->getAsInteger() != lastIco) {
      lastIco = nno_icField->getAsInteger();
      res_pocetZarField->setAsString(countField->getAsString());
      res_nazevPoskField->setAsString(nrzps_nazevPoskField->getAsString());
      res_pravFormField->setAsString(nrzps_pravFormaField->getAsString());
      res_ulicePoskField->setAsString(nrzps_ulicePoskyField->getAsString());
      res_cisloDomPoskyField->setAsString(nrzps_cisloDomPoskField->getAsString());
      res_pscPoskField->setAsString(nrzps_pscPoskField->getAsString());
      res_xField->setAsString(nno_xField->getAsString());
      res_yField->setAsString(nno_yField->getAsString());
      res_nazevZarField->setAsString(nilStr);
      res_obecZarField->setAsString(nilStr);
      res_uliceZarField->setAsString(nilStr);
      res_cisloDomZarField->setAsString(nilStr);
      res_oborPeceField->setAsString(nilStr);
      res_formaPeceField->setAsString(nilStr);
      res_pscZarField->setAsString(nilStr);
      res_orgZarField->setAsString("Poskytovatel zdravotnich sluzeb");
      countRes->dataSet->next();
      setPoskytovatel = true;
    } else {
      res_pocetZarField->setAsString(nilStr);
      res_nazevPoskField->setAsString(nilStr);
      res_pravFormField->setAsString(nilStr);
      res_ulicePoskField->setAsString(nilStr);
      res_cisloDomPoskyField->setAsString(nilStr);
      res_pscPoskField->setAsString(nilStr);

      auto gps =
          Utilities::splitStringByDelimiter(nrzps_gpsField->getAsString(), " ");
      if (gps.size() == 2) {
        res_xField->setAsString(gps[1].substr(0, gps[0].length() - 1));
        res_yField->setAsString(gps[0].substr(1));
      } else {
        res_xField->setAsString(nno_xField->getAsString());
        res_yField->setAsString(nno_yField->getAsString());
      }
      res_orgZarField->setAsString("Zarizeni zdravotnich sluzeb");
      res_nazevZarField->setAsString(nrzps_nazevZarField->getAsString());
      res_obecZarField->setAsString(nrzps_obecZarField->getAsString());
      res_uliceZarField->setAsString(nrzps_uliceZarField->getAsString());
      res_cisloDomZarField->setAsString(nrzps_cisloDomZarField->getAsString());
      res_pscZarField->setAsString(nrzps_pscZarField->getAsString());
      res_oborPeceField->setAsString(nrzps_oborPeceField->getAsString());
      res_formaPeceField->setAsString(nrzps_formaPeceField->getAsString());
    }

    res_krajPoskField->setAsString(nrzps_krajPoskField->getAsString());
    res_okresPoskField->setAsString(nrzps_okresPoskField->getAsString());
    res_obecPoskField->setAsString(nrzps_obecPoskField->getAsString());
    res_obecIdPoskField->setAsString(nno_obecIdField->getAsString());
    res_icField->setAsInteger(lastIco);
    if (setPoskytovatel) {
      goto yay_goto;
    }
  }

  auto writer = DataWriters::XlntWriter("/Users/petr/Desktop/nrzps.xlsx");
  //auto writer = DataWriters::XlsxWriter("/Users/petr/Desktop/nrzps.xlsx");
  //auto writer = DataWriters::CsvWriter("/Users/petr/Desktop/nrzps.csv");
  writer.writeHeader(result.getFieldNames());
  auto fields = result.getFields();
  result.resetBegin();

  while (result.next()) {
    std::vector<std::string> record;
    std::transform(fields.begin(),
                   fields.end(),
                   std::back_inserter(record),
                   [](const DataSets::BaseField *field) {
                     return boost::locale::conv::to_utf<char>(field->getAsString(),
                                                              "CP1250");
                   });
    writer.writeRecord(record);
  }

  std::cout << "done" << std::endl;
}
