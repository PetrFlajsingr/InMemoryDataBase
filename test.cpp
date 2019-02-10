//
// Created by Petr Flajsingr on 2019-02-10.
//

#include <MemoryDataSet.h>

int main() {
  DataSets::MemoryDataSet dataSet("test");

  dataSet.openEmpty({"1", "2"}, {ValueType::String, ValueType::String});

  auto fields = dataSet.getFields();

  for (gsl::index i = 0; i < 20; ++i) {
    dataSet.append();
    for (auto field : fields) {
      field->setAsString(std::to_string(i));
    }
  }

  int cnt = 0;
  for (auto val : dataSet) {
    std::cout << val[0].cells[0]._string << " hihihi" << std::endl;
    cnt++;
  }

  auto tmp = dataSet.begin();

  auto wat = tmp[10];

  return 0;
}