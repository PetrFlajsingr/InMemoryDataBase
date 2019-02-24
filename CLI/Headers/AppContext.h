//
// Created by Petr Flajsingr on 2019-02-23.
//

#ifndef PROJECT_APPCONTEXT_H
#define PROJECT_APPCONTEXT_H

#include <unordered_map>
#include <MemoryDataBase.h>
#include <Config.h>
#include "ConsoleIO.h"

class AppContext {
 public:
  std::unordered_map<std::string, std::shared_ptr<DataBase::MemoryDataBase>>
      DBs;
  enum class Mode {
    query, normal
  };
  Mode mode = Mode::normal;

  Config config = Config("./test.conf");

  template<typename T>
  T getResource(std::string_view resString) {
    if (resString[0] == '['
        && resString.back() == ']'
        && resString[resString.size() - 2] == ']') {
      auto sub = resString.substr(1, resString.size() - 3);
      auto pos = sub.find('[');
      if (pos != std::string::npos) {
        auto cat = sub.substr(0, pos);
        auto key = sub.substr(pos + 1, sub.size() - pos - 1);

        return config.getValue<T>(cat, key);
      }
    }

    throw InvalidArgumentException("Invalid resource format.");
  }

  static AppContext &GetInstance();

 private:

 public:
  AppContext() {};
};

#endif //PROJECT_APPCONTEXT_H
