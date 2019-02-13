//
// Created by Petr Flajsingr on 2019-02-10.
//

#include <MemoryDataSet.h>
#include <LexicalAnalyser.h>
#include <SyntaxAnalyser.h>
#include <SemanticAnalyser.h>

int main() {
  DataBase::LexicalAnalyser lexicalAnalyser;

  std::vector<std::tuple<DataBase::Token, std::string, bool>> tokens;

  lexicalAnalyser.setInput(
      "select#tohle je select# table.a as letadlo, sum(t2.c) as suma, avg(t2.d) as aver "
      "from table "
      "join t2 on table.a = t2.b "
      "outer join t3 on t2.b = t3.b "
      "where t2.b != 10 | 15 or t2.c >= 10000 and letadlo = table.b | 1000 | -100 "
      "group by table.a "
      "having sum(table.a) > 10 and aver > 0.1 "
      "order by t2.c asc, t2.d desc, letadlo asc;");

  try {
    do {
      tokens.push_back(lexicalAnalyser.getNextToken());
    } while (std::get<2>(tokens.back()));

    DataBase::SyntaxAnalyser syntaxAnalyser;
    syntaxAnalyser.setInput(tokens);

    auto strQuery = syntaxAnalyser.analyse();

    DataBase::SemanticAnalyser semanticAnalyser;
    semanticAnalyser.setInput(strQuery);
    semanticAnalyser.analyse();
    std::cout << "Success\n";
  } catch (DataBase::QueryException &exc) {
    std::cerr << exc.what();
  }

  tokens.clear();

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