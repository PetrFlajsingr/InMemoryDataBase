//
// Created by petr on 3/14/20.
//

#include "DotaceForIcos.h"
#include "LoadingUtils.h"
#include <CsvWriter.h>
#include <MemoryDataBase.h>
#include <string>
#include <types/Range.h>
using namespace std::string_literals;
const auto muniPath = "/home/petr/Desktop/muni/"s;
const auto csvPath = "/home/petr/Desktop/muni/"s;
const auto rpssPath = csvPath + "rpss/Prehled_socialnich_sluzeb_CR_2.3.2020.xlsx";
const auto copniSektorPath = csvPath + "INPUT_pf_copni_okres_sektor.xlsx";
int main() {
  auto resNrzps = std::make_shared<DataSets::MemoryDataSet>("res");
  auto res = createDataSetFromFile("res", FileSettings::Csv(muniPath + "res/res_full.csv", ','),
                                   {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                    ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                    ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                    ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                    ValueType::String, ValueType::String, ValueType::String});
  {
    DataBase::MemoryDataBase db{"db"};
    auto resTable = db.addTable(res);
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
    db.addTable(createDataSetFromFile("rpss", FileSettings::Xlsx(rpssPath, "List1"), rpssTypes));
    const auto resQuery = R"(
SELECT res.*
FROM res
JOIN rpss on res.ICO_number = rpss."IČ";
)";
    const auto columnNames = resTable->dataSet->getFieldNames();
    const auto columnTypes = resTable->dataSet->getFieldTypes();
    resNrzps->openEmpty(columnNames, columnTypes);

    auto resDS = db.execSimpleQuery(resQuery, false, "r")->dataSet;
    int lastICO = 0;

    auto srcFields = resDS->getFields();
    auto dstFields = resNrzps->getFields();
    auto srcIcoField = dynamic_cast<DataSets::IntegerField *>(resDS->fieldByName("ICO_number"));
    resDS->resetBegin();
    while(resDS->next()) {
      const auto ico = srcIcoField->getAsInteger();
      if (lastICO == ico) {
        continue;
      }
      resNrzps->append();
      lastICO = ico;
      for (auto i : MakeRange::range(srcFields.size())) {
        dstFields[i]->setAsString(srcFields[i]->getAsString());
      }
    }
  }
  DataBase::MemoryDataBase db{"db"};

  db.addTable(createDataSetFromFile(
      "prav_formy_SEKTOR", FileSettings::Xlsx(copniSektorPath, "prav_formy_SEKTOR"),
      {ValueType::String,  ValueType::String, ValueType::String, ValueType::String, ValueType::String}));

  db.addTable(exportForRes(resNrzps));
  db.addTable(res);
  const auto querySektor = R"(
SELECT
dotace.*,
prav_formy_SEKTOR.sub_sektor,
prav_formy_SEKTOR.sektor
FROM dotace
LEFT JOIN res ON dotace.ICO_number = res.ICO_number
LEFT JOIN prav_formy_SEKTOR ON res.FORMA = prav_formy_SEKTOR.NNO;
)";
  auto dotaceDS = db.execSimpleQuery(querySektor, false, "fin")->dataSet;
  DataWriters::CsvWriter writerFull{csvPath + "rpss_fin_full.csv"};
  writerFull.setAddQuotes(true);
  DataWriters::CsvWriter writerGis{csvPath + "rpss_fin_gis.csv"};
  writerGis.setAddQuotes(true);
  auto aktivniField = dotaceDS->fieldByName("INSTITUCE_V_LIKVIDACI");
  dotaceDS->resetBegin();

  auto fields = dotaceDS->getFields();
  std::vector<std::string> row;
  const auto header = dotaceDS->getFieldNames();
  writerFull.writeHeader(header);
  writerGis.writeHeader(header);
  while(dotaceDS->next()) {
    row.clear();
    std::transform(fields.begin(), fields.end(), std::back_inserter(row), [] (const auto &field) {
      return field->getAsString();
    });
    if (aktivniField->getAsString() == "aktivní") {
      writerGis.writeRecord(row);
    }
    writerFull.writeRecord(row);
  }
  return 0;
}
