//
// Created by Petr Flajsingr on 2019-02-23.
//

#ifndef PROJECT_CLICONTROLLER_H
#define PROJECT_CLICONTROLLER_H

#include "ScriptParser.h"
#include "AppContext.h"
#include <MessageReceiver.h>

class CLIController : public MessageReceiver, public MessageSender {
 public:
  CLIController();
  void runApp();
  static void RunScript(std::string_view scriptPath);

 private:
  void receive(std::shared_ptr<Message> message) override;
  enum class CmdType {
    runScript,
    queryModeStart, // query start/qs
    queryModeEnd, // >> query end/>> qe
    clear, // clear
    help, // help/h/?
    exit, // exit/quit/e/q
    listDBs,
    listTables,
    download,
    downloadAsync,
    unknown
  };
  ScriptParser inputParser = ScriptParser::GetInstance();

  std::string scriptPath;
  std::string dbName;

  std::string fileName;
  std::string savePath;

  void handleQuery(std::string_view query);
  CmdType handleCommand(std::string_view command);

  void handleInput(std::string_view input);

  void printHelp();
};

#endif //PROJECT_CLICONTROLLER_H
