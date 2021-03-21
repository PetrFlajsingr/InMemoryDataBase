//
// Created by petr on 3/9/21.
//

#ifndef CSV_READER__DISTINCT_COPNI_H
#define CSV_READER__DISTINCT_COPNI_H

#include <types/Range.h>
#include <various/isin.h>
inline std::shared_ptr<DataSets::MemoryDataSet> distinctCopniCodes(DataSets::BaseDataSet &ds) {
  auto result = std::make_shared<DataSets::MemoryDataSet>("copni2");
  result->openEmpty(ds.getFieldNames(),
                    {ValueType::String, ValueType::String, ValueType::String, ValueType::String, ValueType::String});
  DataSets::SortOptions sortOptions;
  sortOptions.addOption(ds.fieldByName("COPNI_codes"), SortOrder::Ascending);
  ds.sort(sortOptions);

  std::vector<std::string> containedCodes;
  ds.resetBegin();
  auto codeField = ds.fieldByName("COPNI_codes");
  auto srcFields = ds.getFields();
  auto dstFields = result->getFields();
  while (ds.next()) {
    if (isIn(codeField->getAsString(), containedCodes)) {
      continue;
    }
    containedCodes.emplace_back(codeField->getAsString());
    result->append();
    for (auto i : MakeRange::range(srcFields.size())) {
      dstFields[i]->setAsString(srcFields[i]->getAsString());
    }
  }
  return result;
}

#endif // CSV_READER__DISTINCT_COPNI_H
