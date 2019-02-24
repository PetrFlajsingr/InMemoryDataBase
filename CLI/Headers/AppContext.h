//
// Created by Petr Flajsingr on 2019-02-23.
//

#ifndef PROJECT_APPCONTEXT_H
#define PROJECT_APPCONTEXT_H

#include <unordered_map>
#include <MemoryDataBase.h>
#include "ConsoleIO.h"

class AppContext {
 public:
  std::unordered_map<std::string, std::shared_ptr<DataBase::MemoryDataBase>>
      DBs;
  enum class Mode {
    query, normal
  };
  Mode mode;

  static AppContext &GetInstance();

 private:
 public:
  AppContext();
};

#endif //PROJECT_APPCONTEXT_H
