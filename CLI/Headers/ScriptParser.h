//
// Created by Petr Flajsingr on 2019-02-23.
//

#ifndef PROJECT_SCRIPTPARSER_H
#define PROJECT_SCRIPTPARSER_H

#include <string_view>
#include <vector>
#include <string>
#include <Types.h>

// TODO: resource string
class ScriptParser {
 public:
  enum class SaveMode {
    normal
  };

  enum class FileTypes {
    csv, xlsx
  };

  enum class Command {
    createDB, removeDB,
    load, append,
    removeTab,
    query, save, unknown
  };

  static ScriptParser &GetInstance();

  bool runCommand(Command command);

  Command parseInput(std::string_view input);

  std::string dbName;
  std::string query;
  std::string filePath;
  std::string dataSetName;

  std::vector<ValueType> valueTypes;

  FileTypes fileType;
  SaveMode saveMode;

  std::string delimiter;

  static std::string ReplaceResources(std::string input);
 private:
  std::vector<std::string> tokenize(std::string_view input);
};

#endif //PROJECT_SCRIPTPARSER_H
