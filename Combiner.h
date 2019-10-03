//
// Created by Petr Flajsingr on 2019-06-22.
//

#ifndef CSV_READER_COMBINER_H
#define CSV_READER_COMBINER_H

#include <vector>
#include <memory>
#include <utility>
#include <string>
#include <map>

#include <MemoryDataBase.h>
#include <Exceptions.h>

using namespace std::string_literals;

class Combiner {
 public:
    void addDataSource(const std::shared_ptr<DataBase::View> &view);

    std::shared_ptr<DataBase::Table> combineOn(const std::map<std::string, std::string> &tableAndColumn);

 private:
  std::vector<std::shared_ptr<DataBase::View>> views;

  void sortDataSets(const std::map<std::string, std::string> &tableAndColumn);

  std::pair<std::vector<std::shared_ptr<DataBase::View>>, std::vector<DataSets::BaseField *>>
  prepareJoinFields(const std::map<std::string,
                                   std::string> &tableAndColumn);

  std::shared_ptr<DataBase::View> viewByName(const std::string &name);
  std::pair<std::vector<std::string>, std::vector<ValueType>> prepareColumnInfo(const std::map<std::string,
                                                                                               std::string> &tableAndColumn);

};

#endif //CSV_READER_COMBINER_H
