//
// Created by petr on 10/10/19.
//

#ifndef CSV_READER_DATASETPROVIDER_H
#define CSV_READER_DATASETPROVIDER_H

#include "BaseDataProvider.h"
#include <BaseDataSet.h>

namespace DataProviders {
class DataSetProvider : public BaseDataProvider {
public:
  DataSetProvider(DataSets::BaseDataSet &dataSet);
  [[nodiscard]] const std::vector<std::string> &getRow() const override;
  [[nodiscard]] std::string getColumnName(unsigned int columnIndex) const override;
  [[nodiscard]] uint64_t getColumnCount() const override;
  [[nodiscard]] const std::vector<std::string> &getHeader() const override;
  [[nodiscard]] int getCurrentRecordNumber() const override;
  bool next() override;
  void first() override;
  [[nodiscard]] bool eof() const override;

private:
  DataSets::BaseDataSet &dataSet;

  std::vector<std::string> currentRow;
  std::vector<std::string> header;

  void readHeader();

  void readRow();
};
}

#endif // CSV_READER_DATASETPROVIDER_H
