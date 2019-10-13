//
// Created by petr on 10/10/19.
//

#include "DataSetProvider.h"

DataProviders::DataSetProvider::DataSetProvider(DataSets::BaseDataSet &dataSet) : dataSet(dataSet) {
  readHeader();
}


const std::vector<std::string> &DataProviders::DataSetProvider::getRow() const {
  return currentRow;
}

std::string DataProviders::DataSetProvider::getColumnName(unsigned int columnIndex) const {
  return header[columnIndex];
}

uint64_t DataProviders::DataSetProvider::getColumnCount() const {
  return header.size();
}

const std::vector<std::string> &DataProviders::DataSetProvider::getHeader() const {
  return header;
}

int DataProviders::DataSetProvider::getCurrentRecordNumber() const {
  return dataSet.getCurrentRecord();
}

bool DataProviders::DataSetProvider::next() {
  if (dataSet.next()) {
    readRow();
    return true;
  }
  return false;
}

void DataProviders::DataSetProvider::first() {
  dataSet.resetBegin();
}

bool DataProviders::DataSetProvider::eof() const {
  return dataSet.isEnd();
}

void DataProviders::DataSetProvider::readHeader() {
  header = dataSet.getFieldNames();
}

void DataProviders::DataSetProvider::readRow() {
  currentRow.clear();
  auto fields = dataSet.getFields();
  std::transform(fields.begin(), fields.end(), std::back_inserter(currentRow), [](auto &field) {return field->getAsString(); });
}
