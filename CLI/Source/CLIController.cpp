//
// Created by Petr Flajsingr on 2019-02-23.
//

#include <CLIController.h>

#include "CLIController.h"
#include <CsvReader.h>
#include <XlsReader.h>
#include <XlsxWriter.h>

CLIController::CLIController() = default;

void CLIController::runApp() {
  AppContext::GetInstance().mode = AppContext::Mode::normal;

  ConsoleIO::writeLn("Normal mode");
  ConsoleIO::writeLn("___________");

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    auto input = ConsoleIO::readLn();
    if (input.length() > 0) {
      if (AppContext::GetInstance().mode == AppContext::Mode::normal) {
        if (input[0] == ':') {
          handleQuery(input.substr(1));
        } else {
          switch (handleCommand(input)) {
            case CmdType::exit:return;
            case CmdType::queryModeStart:
              AppContext::GetInstance().mode = AppContext::Mode::query;
              ConsoleIO::setMode(ConsoleIO::Mode::arrow);
              ConsoleIO::writeLn("Query mode");
              ConsoleIO::writeLn("**********");
              break;
            case CmdType::queryModeEnd:
              AppContext::GetInstance().mode = AppContext::Mode::normal;
              ConsoleIO::setMode(ConsoleIO::Mode::simple);
              ConsoleIO::writeLn("Normal mode");
              ConsoleIO::writeLn("\"***********\"");
              break;
            case CmdType::clear:AppContext::GetInstance().DBs.clear();
              break;
            case CmdType::help:printHelp();
              break;
            case CmdType::runScript:RunScript(scriptPath);
              break;
            case CmdType::listDBs:ConsoleIO::writeLn("List of DataBases:");
              for (const auto &db : AppContext::GetInstance().DBs) {
                ConsoleIO::writeLn(db.first);
              }
              break;
            case CmdType::listTables:
              ConsoleIO::writeLn("List of tables in " + dbName + ": ");
              if (AppContext::GetInstance().DBs.find(dbName)
                  == AppContext::GetInstance().DBs.end()) {
                ConsoleIO::writeLnErr("DataBase not found.");
                break;
              }
              for (const auto
                    &val : AppContext::GetInstance().DBs[dbName]->getTables()) {
                ConsoleIO::writeLn(val->getName());
              }
              break;
          }
        }
      } else {
        if (input[0] == ':') {
          switch (handleCommand(input.substr(1))) {
            case CmdType::queryModeEnd:
              AppContext::GetInstance().mode = AppContext::Mode::normal;
              ConsoleIO::setMode(ConsoleIO::Mode::simple);
              ConsoleIO::writeLn("Normal mode");
              ConsoleIO::writeLn("***********");
              break;
            case CmdType::clear:AppContext::GetInstance().DBs.clear();
              break;
            case CmdType::help:printHelp();
              break;
            default:break;
          }
        } else {
          handleQuery(input);
        }
      }
    }
  }
}

void CLIController::handleQuery(std::string_view query) {
  auto cmd = inputParser.parseInput(query);
  try {
    inputParser.runCommand(cmd);
  } catch (const std::exception &e) {
    ConsoleIO::writeLnErr("Command could not be performed.\n"
                              + std::string(typeid(e).name()) + "\n"
                              + std::string(e.what()));
  }
  ConsoleIO::write("");
}

CLIController::CmdType CLIController::handleCommand(std::string_view command) {
  if (Utilities::compareString(command, "exit") == 0
      || Utilities::compareString(command, "e") == 0
      || Utilities::compareString(command, "quit") == 0
      || Utilities::compareString(command, "q") == 0) {
    return CmdType::exit;
  }
  if (Utilities::compareString(command, "query start") == 0
      || Utilities::compareString(command, "qs") == 0) {
    return CmdType::queryModeStart;
  }
  if (Utilities::compareString(command, "query end") == 0
      || Utilities::compareString(command, "qe") == 0) {
    return CmdType::queryModeEnd;
  }
  if (Utilities::compareString(command, "help") == 0
      || Utilities::compareString(command, "h") == 0
      || Utilities::compareString(command, "?") == 0) {
    return CmdType::help;
  }
  if (Utilities::compareString(command, "clear") == 0
      || Utilities::compareString(command, "c") == 0) {
    return CmdType::clear;
  }
  if (command.substr(0, 10) == "run script") {
    scriptPath = command.substr(11);
    return CmdType::runScript;
  }
  if (command == "list dbs" || command == "ldb") {
    return CmdType::listDBs;
  }
  if (command.substr(0, 11) == "list tables") {
    dbName = Utilities::splitStringByDelimiter(command, " ")[1];
    return CmdType::listTables;
  }
  if (command.substr(0, 4) == "ltbl") {
    dbName = Utilities::splitStringByDelimiter(command, " ")[1];
    return CmdType::listTables;
  }
  ConsoleIO::writeLn("Ignoring unknown command.");
  return CmdType::unknown;
}

void CLIController::printHelp() {
  ConsoleIO::write("exit\t\texit/e/quit\n"
                   "query mode\tquery start/qs\n"
                   "query end\tquery end/qe\n"
                   "clear\t\tclear/c\n");
}

void CLIController::RunScript(std::string_view scriptPath) {
  auto path = std::string(scriptPath);
  std::ifstream input(path);
  auto scriptRunner = ScriptParser::GetInstance();
  if (!input.is_open()) {
    ConsoleIO::writeLnErr("Can't open script file.");
  }
  std::string line;
  try {
    while (std::getline(input, line)) {
      ConsoleIO::writeLn("Running: " + line);
      if (!scriptRunner.runCommand(scriptRunner.parseInput(line))) {
        ConsoleIO::writeLnErr("Interrupting script execution");
        break;
      }
    }
    ConsoleIO::writeLn("Script finished successfully.");
  } catch (const std::exception &e) {
    ConsoleIO::writeLnErr("Command could not be performed.\n"
                              + std::string(typeid(e).name()) + "\n"
                              + std::string(e.what()));
  }
  ConsoleIO::write("");
}
