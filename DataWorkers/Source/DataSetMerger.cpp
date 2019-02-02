//
// Created by Petr Flajsingr on 2019-01-25.
//

#include <DataSetMerger.h>
#include <DateTimeField.h>

DataWorkers::DataSetMerger::DataSetMerger() {}

void DataWorkers::DataSetMerger::removeDataSet(std::string_view dataSetName) {
  auto foundDataSet = std::find_if(dataSets.begin(),
                                   dataSets.end(),
                                   [&dataSetName](DataSets::BaseDataSet *dataSet) {
                                     return dataSet->getName() == dataSetName;
                                   });

  if (foundDataSet != dataSets.end()) {
    dataSets.erase(foundDataSet);
  }
}
gsl::not_null<DataSets::BaseDataSet *> DataWorkers::DataSetMerger::mergeDataSets(
    std::string_view dataSetName1,
    std::string_view dataSetName2,
    std::string_view columnName1,
    std::string_view columnName2) {
  // find required data sets
  auto dataSet1Iterator = std::find_if(dataSets.begin(),
                                       dataSets.end(),
                                       [&dataSetName1](DataSets::BaseDataSet *dataSet) {
                                         return dataSet->getName() == dataSetName1;
                                       });

  if (dataSet1Iterator == dataSets.end()) {
    std::string strName(dataSetName1);
    std::string errMsg = "DataSet named \"" + strName + "\" does not exist.";
    throw InvalidArgumentException(errMsg.c_str());
  }

  auto dataSet1 = dynamic_cast<DataSets::MemoryDataSet *>(*dataSet1Iterator);

  auto dataSet2Iterator = std::find_if(dataSets.begin(),
                                       dataSets.end(),
                                       [&dataSetName2](DataSets::BaseDataSet *dataSet) {
                                         return dataSet->getName() == dataSetName2;
                                       });

  if (dataSet2Iterator == dataSets.end()) {
    std::string strName(dataSetName2);
    std::string errMsg = "DataSet named \"" + strName + "\" does not exist.";
    throw InvalidArgumentException(errMsg.c_str());
  }

  auto dataSet2 = dynamic_cast<DataSets::MemoryDataSet *>(*dataSet2Iterator);

  // prepare result data set fields and open it -- all fields of both merged data sets
  std::string strName1(dataSetName1);
  std::string strName2(dataSetName2);
  auto resultDataSetName = strName1 + "_" + strName2;
  auto result = new DataSets::MemoryDataSet(resultDataSetName);
  std::vector<ValueType> fieldTypes;
  std::vector<std::string> fieldNames;
  std::vector<DataSets::BaseField *> sourceFields;

  for (auto field : dataSet1->getFields()) {
    fieldTypes.emplace_back(field->getFieldType());
    fieldNames.emplace_back(field->getFieldName());
    sourceFields.emplace_back(field);
  }
  for (auto field : dataSet2->getFields()) {
    if (field->getFieldName() == columnName2) {
      continue;
    }
    fieldTypes.emplace_back(field->getFieldType());
    fieldNames.emplace_back(field->getFieldName());
    sourceFields.emplace_back(field);
  }
  result->setFieldTypes(fieldNames, fieldTypes);

  result->openEmpty();

  // sort merging datasets
  auto mergeField1 = dataSet1->fieldByName(columnName1);
  auto mergeField2 = dataSet2->fieldByName(columnName2);

  if (mergeField1->getFieldType() != mergeField2->getFieldType()) {
    std::string errMsg = "Required merge field are not of the same data type.";
    throw InvalidArgumentException(errMsg.c_str());
  }

  if (mergeField1->getFieldType() != ValueType::Integer
      & mergeField1->getFieldType() != ValueType::String) {
    std::string errMsg = "Unsupported field type for merging (not integer nor string).";
    throw InvalidArgumentException(errMsg.c_str());
  }

  auto isIntegerField = mergeField1->getFieldType() == ValueType::Integer;

  DataSets::SortOptions options1;
  options1.addOption(mergeField1->getIndex(), SortOrder::Ascending);
  dataSet1->sort(options1);

  DataSets::SortOptions options2;
  options2.addOption(mergeField2->getIndex(), SortOrder::Ascending);
  dataSet2->sort(options2);

  int8_t cmpResult;
  while (!dataSet1->eof()) {

    while (!dataSet2->eof()) {
      if (isIntegerField) {
        cmpResult = Utilities::compareInt(reinterpret_cast<DataSets::IntegerField *>(mergeField1)->getAsInteger(),
                                          reinterpret_cast<DataSets::IntegerField *>(mergeField2)->getAsInteger());
      } else {
        cmpResult = Utilities::compareString(mergeField1->getAsString(),
                                             mergeField2->getAsString());
      }
      if (cmpResult == 0) {
        appendData(sourceFields, result);
        //dataSet2->next();
        break;
      } else if (cmpResult < 0) {
        break;
      } else if (cmpResult > 0) {
        dataSet2->next();
      }
    }
    if (dataSet2->eof()) {
      break;
    }

    dataSet1->next();
  }

  result->first();
  return result;
}

void DataWorkers::DataSetMerger::appendData(std::vector<DataSets::BaseField *> fields,
                                            DataSets::BaseDataSet *dataset) {
  dataset->append();
  auto dataSetFields = dataset->getFields();

  for (auto i = 0; i < fields.size(); ++i) {
    switch (dataSetFields[i]->getFieldType()) {
      case ValueType::Integer:
        reinterpret_cast<DataSets::IntegerField *>(dataSetFields[i])
            ->setAsInteger(reinterpret_cast<DataSets::IntegerField *>(fields[i])->getAsInteger());
        break;
      case ValueType::Double:
        reinterpret_cast<DataSets::DoubleField *>(dataSetFields[i])
            ->setAsDouble(reinterpret_cast<DataSets::DoubleField *>(fields[i])->getAsDouble());
        break;
      case ValueType::Currency: {
        auto value = reinterpret_cast<DataSets::CurrencyField *>(fields[i])->getAsCurrency();
        reinterpret_cast<DataSets::CurrencyField *>(dataSetFields[i])
            ->setAsCurrency(value);
      }
        break;
      case ValueType::String:dataSetFields[i]->setAsString(fields[i]->getAsString());
        break;
      case ValueType::DateTime: {
        auto value = reinterpret_cast<DataSets::DateTimeField *>(fields[i])->getAsDateTime();
        reinterpret_cast<DataSets::DateTimeField *>(dataSetFields[i])
            ->setAsDateTime(value);
      }
        break;
      default:throw IllegalStateException("Internal error, DataWorkers::DataSetMerger::appendData(...)");
    }
  }
}
