//
// Created by Petr Flajsingr on 2019-02-10.
//

#include <MemoryDataSet.h>
#include <LexicalAnalyser.h>
#include <SyntaxAnalyser.h>

int main() {
  DataBase::LexicalAnalyser lexicalAnalyser;
  lexicalAnalyser.setInput(
      "select table.a, sum(t2.c), avg(t2.d) from table join t2 on table.a = t2.b where t2.b != 10 "
      "group by table.a having a.a > 10 order by t2.c asc;");

  std::vector<std::tuple<DataBase::Token, std::string, bool>> tokens;

  do {
    tokens.push_back(lexicalAnalyser.getNextToken());
  } while (std::get<2>(tokens.back()));

  DataBase::SyntaxAnalyser syntaxAnalyser;
  syntaxAnalyser.setInput(tokens);

  syntaxAnalyser.analyse();

  return 0;
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

  std::cout << "____________SORT 0 DESC, FILTER 1 2 3________________"
            << std::endl;

  DataSets::FilterOptions options;
  options.addOption(dataSet.fieldByIndex(0),
                    {"1", "2", "3"},
                    DataSets::FilterOption::Equals);
  auto view = dataSet.filter(options);

  auto viewFields = view->getFields();

  DataSets::SortOptions sortOptions;
  sortOptions.addOption(viewFields[0],
                        SortOrder::Descending);
  view->sort(sortOptions);

  while (view->next()) {
    for (const auto field : viewFields) {
      std::cout << field->getAsString() << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "_____________FILTER 1 2 5_______________" << std::endl;

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

  std::cout << "______________SORT 0 ASC______________" << std::endl;

  DataSets::SortOptions sortOptions2;
  sortOptions2.addOption(viewFields[0],
                         SortOrder::Ascending);
  view->sort(sortOptions2);

  view->resetBegin();
  while (view->next()) {
    for (const auto field : viewFields) {
      std::cout << field->getAsString() << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "_______________FILTER 1 2 5 AGAIN_____________" << std::endl;

  view2->resetBegin();
  while (view2->next()) {
    for (const auto field : view2Fields) {
      std::cout << field->getAsString() << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}