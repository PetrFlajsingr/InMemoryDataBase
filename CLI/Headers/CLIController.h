//
// Created by Petr Flajsingr on 2019-02-23.
//

#ifndef PROJECT_CLICONTROLLER_H
#define PROJECT_CLICONTROLLER_H

#include "ScriptParser.h"
#include "AppContext.h"

class CLIController {
 public:
  CLIController();
  void runApp();
  static void RunScript(std::string_view scriptPath);
 private:
  enum class CmdType {
    runScript,
    queryModeStart, // query start/qs
    queryModeEnd, // >> query end/>> qe
    clear, // clear
    help, // help/h/?
    exit, // exit/quit/e/q
    listDBs,
    listTables,
    unknown
  };
  ScriptParser inputParser = ScriptParser::GetInstance();

  std::string scriptPath;
  std::string dbName;

  void handleQuery(std::string_view query);
  CmdType handleCommand(std::string_view command);

  void printHelp();
};

#endif //PROJECT_CLICONTROLLER_H
