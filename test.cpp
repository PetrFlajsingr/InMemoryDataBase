//
// Created by Petr Flajsingr on 2019-02-10.
//

#include <MemoryDataSet.h>
#include <MemoryViewDataSet.h>

int main() {
  DataSets::MemoryDataSet dataSet("test");

  dataSet.openEmpty({"1", "2"}, {ValueType::String, ValueType::String});

  auto fields = dataSet.getFields();

  int j = 0;
  for (gsl::index i = 0; i < 20; ++i) {
    dataSet.append();
    for (auto field : fields) {
      field->setAsString(std::to_string(j));
    }
    if (i % 5 == 0) {
      j++;
    }
  }

  j = 0;
  for (gsl::index i = 0; i < 20; ++i) {
    dataSet.append();
    for (auto field : fields) {
      field->setAsString(std::to_string(j));
    }
    if (i % 5 == 0) {
      j++;
    }
  }

  dataSet.resetBegin();
  while (dataSet.next()) {
    for (const auto field : dataSet.getFields()) {
      std::cout << field->getAsString() << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "____________________________" << std::endl;

  DataSets::FilterOptions options;
  options.addOption(dataSet.fieldByIndex(0),
                    {"1", "2", "3"},
                    DataSets::FilterOption::Equals);
  auto view = dataSet.filter(options);

  auto viewFields = view->getFields();

  while (view->next()) {
    for (const auto field : viewFields) {
      std::cout << field->getAsString() << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "____________________________" << std::endl;

  DataSets::FilterOptions options2;
  options2.addOption(view->fieldByIndex(0),
                     {"1", "2", "5"},
                     DataSets::FilterOption::Equals);

  auto view2 = view->filter(options2);

  auto view2Fields = view2->getFields();

  while (view2->next()) {
    for (const auto field : view2Fields) {
      std::cout << field->getAsString() << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}