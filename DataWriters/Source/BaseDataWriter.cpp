//
// Created by petr on 12/7/19.
//

#include "BaseDataWriter.h"

void DataWriters::BaseDataWriter::writeDataSet(DataSets::BaseDataSet &dataSet) {
  dataSet.resetBegin();
  writeHeader(dataSet.getFieldNames());
  auto fields = dataSet.getFields();
  while (dataSet.next()) {
    std::vector<std::string> row;
    for (auto &field : fields) {
      row.emplace_back(field->getAsString());
    }
    writeRecord(row);
  }
}