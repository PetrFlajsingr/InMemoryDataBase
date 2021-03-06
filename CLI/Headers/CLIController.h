//
// Created by Petr Flajsingr on 2019-02-23.
//

#ifndef PROJECT_CLICONTROLLER_H
#define PROJECT_CLICONTROLLER_H

#include "AppContext.h"
#include "ScriptParser.h"
#include <MessageReceiver.h>
#include <memory>
#include <string>

// TODO: unarchive, async query, async unarchive, wait async query/unarchive
class CLIController : public MessageReceiver, public MessageSender {
public:
  CLIController();
  void runApp();
  /**
   * Run a query script from file
   * @param scriptPath path to query script file
   */
  static void RunScript(std::string_view scriptPath);

private:
  void receive(std::shared_ptr<Message> message) override;
  enum class CmdType {
    runScript,
    queryModeStart, // query start/qs
    queryModeEnd,   // >> query end/>> qe
    clear,          // clear
    help,           // help/h/?
    exit,           // exit/quit/e/q
    listDBs,
    listTables,
    download,
    downloadAsync,
    waitDownload,
    unknown
  };
  ScriptParser inputParser = ScriptParser::GetInstance();

  std::string scriptPath;
  std::string dbName;

  std::string fileName;
  std::string savePath;

  AppContext &ctx = AppContext::GetInstance();

  void handleQuery(std::string_view query);
  CmdType handleCommand(std::string_view input);

  void handleInput(std::string_view input);

  void printHelp();
};

#endif // PROJECT_CLICONTROLLER_H
