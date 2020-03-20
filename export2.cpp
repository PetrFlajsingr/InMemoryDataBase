//
// Created by petr on 10/7/19.
//

#include "DotaceForIcos.h"
#include "LoadingUtils.h"
#include <CsvWriter.h>
#include <string>
using namespace std::string_literals;
const auto muniPath = "/home/petr/Desktop/muni/"s;
const auto csvPath = "/home/petr/Desktop/muni/"s;
int main() {
  auto dotaceDS = exportForRes(createDataSetFromFile("res", FileSettings::Csv(muniPath + "res/res.csv", ','),
                                  {ValueType::Integer, ValueType::String, ValueType::String, ValueType::String,
                                   ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                   ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                   ValueType::String, ValueType::String, ValueType::String, ValueType::String,
                                   ValueType::String, ValueType::String, ValueType::String}));
  DataWriters::CsvWriter writerFull{csvPath + "nno_fin_full.csv"};
  writerFull.setAddQuotes(true);
  DataWriters::CsvWriter writerGis{csvPath + "nno_fin_gis.csv"};
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
