//
// Created by Petr Flajsingr on 2019-06-22.
//

#include "Combiner.h"

std::pair<std::vector<std::string>,
          std::vector<ValueType>> Combiner::prepareColumnInfo(const std::map<std::string,
                                                                             std::string> &tableAndColumn) {
  std::pair<std::vector<std::string>, std::vector<ValueType>> result;
  for (auto[name, _] : tableAndColumn) {
    if (auto view = viewByName(name)) {
      for (const auto &field : view->dataSet->getFields()) {
        result.first.emplace_back(name + "$" + field->getName().data());
        result.second.emplace_back(field->getFieldType());
      }
    } else {
      throw InvalidArgumentException("No table or view named '"s + name + "' in Combiner");
    }
  }
  return result;
}

std::shared_ptr<DataBase::View> Combiner::viewByName(const std::string &name) {
  auto iter = std::find_if(views.begin(), views.end(), [&name](auto &view) {
    bool res = Utilities::compareString(view->getName(), name) == 0;
    return res;
  });
  if (iter != views.end()) {
    return *iter;
  }
  return nullptr;
}

void Combiner::addDataSource(const std::shared_ptr<DataBase::View> &view) {
  views.emplace_back(view);
}

void Combiner::sortDataSets(const std::map<std::string, std::string> &tableAndColumn) {
  for (auto[name, column] : tableAndColumn) {
    auto view = viewByName(name);
    DataSets::SortOptions options;
    options.addOption(view->dataSet->fieldByName(column), SortOrder::Ascending);
    view->dataSet->sort(options);
  }
}

std::pair<std::vector<std::shared_ptr<DataBase::View>>, std::vector<DataSets::BaseField *>>
Combiner::prepareJoinFields(const std::map<std::string,
                                           std::string>
                            &tableAndColumn) {
  std::vector<DataSets::BaseField *> joinFields;
  std::vector<std::shared_ptr<DataBase::View>> views;
  for (auto[view, field] : tableAndColumn) {
    views.emplace_back(viewByName(view));
    joinFields.emplace_back(viewByName(view)->dataSet->fieldByName(field));
  }
  return std::make_pair(views, joinFields);
}

std::shared_ptr<DataBase::Table> Combiner::combineOn(const std::map<std::string, std::string> &tableAndColumn) {
  sortDataSets(tableAndColumn);

  auto dataSet = std::make_shared<DataSets::MemoryDataSet>("result");
  auto[fieldNames, fieldTypes] = prepareColumnInfo(tableAndColumn);
  dataSet->openEmpty(fieldNames, fieldTypes);

  auto result = std::make_shared<DataBase::Table>(dataSet);

  auto[views, joinFields] = prepareJoinFields(tableAndColumn);

  for (const auto& view : views) {
    view->dataSet->next();
  }

  views[0]->dataSet->resetBegin();

  while (views[0]->dataSet->next()) {
    int colOffset = views[0]->dataSet->getColumnCount();
    std::vector<std::string> newRow;
    newRow.resize(dataSet->getColumnCount());

    {
      int i = 0;
      for (auto field : views[0]->dataSet->getFields()) {
        newRow[i] = field->getAsString();
        ++i;
      }
    }

    int fieldCnt = 1;
    for (auto iter = views.begin() + 1; iter != views.end(); ++iter) {
      std::cout << joinFields[0]->getAsString() << std::endl;
      switch (Utilities::compareString(joinFields[fieldCnt]->getAsString(), joinFields[0]->getAsString())) {
        case -1:
          if ((*iter)->dataSet->next()) {
            --iter;
            continue;
          }
          break;
        case 0: {
          std::vector<std::string> toAppend = newRow;
          int i = colOffset;
          for (auto field : (*iter)->dataSet->getFields()) {
            toAppend[i] = field->getAsString();
            ++i;
          }

          int cnt = 0;
          dataSet->append();
          for (auto field : dataSet->getFields()) {
            field->setAsString(toAppend[cnt]);
            cnt++;
          }

          if ((*iter)->dataSet->next()) {
            --iter;
            continue;
          }
        }
          break;
        case 1:break;
      }
      colOffset += (*iter)->dataSet->getColumnCount();
      fieldCnt++;
    }
  }

  result->dataSet->resetBegin();
  return result;
}
