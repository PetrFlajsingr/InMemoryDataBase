//
// Created by Petr Flajsingr on 2019-02-15.
//

#ifndef PROJECT_MOCKDATASET_H
#define PROJECT_MOCKDATASET_H

class MockDataSet : public DataSets::BaseDataSet {
 public:
  explicit MockDataSet(const std::string_view &dataSetName) : BaseDataSet(
      dataSetName) {}

  void open(DataProviders::BaseDataProvider &dataProvider,
            const std::vector<ValueType> &fieldTypes) override {
    throw NotImplementedException();
  }
  void openEmpty(const std::vector<std::string> &fieldNames,
                 const std::vector<ValueType> &fieldTypes) override {
    for (gsl::index i = 0; i < fieldNames.size(); ++i) {
      fields.emplace_back(DataSets::FieldFactory::Get().CreateField(
          fieldNames[i],
          i,
          fieldTypes[i],
          this));
    }
  }
  void close() override {
    throw NotImplementedException();
  }
  void first() override {
    throw NotImplementedException();
  }
  void last() override {
    throw NotImplementedException();
  }
  bool next() override {
    throw NotImplementedException();
  }
  bool previous() override {
    throw NotImplementedException();
  }
  bool isFirst() const override {
    throw NotImplementedException();
  }
  bool isBegin() const override {
    throw NotImplementedException();
  }
  bool isLast() const override {
    throw NotImplementedException();
  }
  bool isEnd() const override {
    throw NotImplementedException();
  }

  DataSets::BaseField *fieldByName(std::string_view name) const override {
    for (auto &field : fields) {
      if (Utilities::compareString(field->getName(), name) == 0) {
        return field.get();
      }
    }
    std::string errMsg = "Field named \"" + std::string(name)
        + "\" not found. DataSets::MemoryDataSet::fieldByName";
    throw InvalidArgumentException(errMsg.c_str());
  }

  DataSets::BaseField *fieldByIndex(gsl::index index) const override {
    throw NotImplementedException();
  }

  std::vector<DataSets::BaseField *> getFields() const override {
    std::vector<DataSets::BaseField *> result;
    for (const auto &field : fields) {
      result.emplace_back(field.get());
    }
    return result;
  }
  std::vector<std::string> getFieldNames() const override {
    std::vector<std::string> result;
    for (const auto &field : fields) {
      result.emplace_back(field->getName());
    }
    return result;
  }
  gsl::index getCurrentRecord() const override {
    throw NotImplementedException();
  }
  void append() override {
    throw NotImplementedException();
  }
  void append(DataProviders::BaseDataProvider &dataProvider) override {
    throw NotImplementedException();
  }
  void sort(DataSets::SortOptions &options) override {
    throw NotImplementedException();
  }
  std::shared_ptr<DataSets::ViewDataSet> filter(const DataSets::FilterOptions &options) override {
    throw NotImplementedException();
  }
  bool findFirst(DataSets::FilterItem &item) override {
    throw NotImplementedException();
  }
  void resetBegin() override {
    throw NotImplementedException();
  }
  void resetEnd() override {
    throw NotImplementedException();
  }
 protected:
  void setData(void *data, gsl::index index, ValueType type) override {
    throw NotImplementedException();
  }
};

#endif //PROJECT_MOCKDATASET_H
