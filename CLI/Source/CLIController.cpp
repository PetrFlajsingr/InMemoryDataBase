//
// Created by Petr Flajsingr on 2019-02-23.
//

#include <CLIController.h>

#include "CLIController.h"
#include <CsvReader.h>
#include <XlsReader.h>
#include <XlsxWriter.h>

CLIController::CLIController()
    : MessageSender(AppContext::GetInstance().getMessageManager()) {}

void CLIController::runApp() {
  AppContext::GetInstance().getMessageManager()->registerMessage<StdinMsg>(
      this);
  AppContext::GetInstance().getMessageManager()->registerMessage<
      DownloadProgress>(
      this);

  AppContext::GetInstance().mode = AppContext::Mode::normal;

  AppContext::GetInstance().getUserInterface()->writeLn("Normal mode");
  AppContext::GetInstance().getUserInterface()->writeLn("___________");
  AppContext::GetInstance().getUserInterface()->listenInput();
}

void CLIController::handleQuery(std::string_view query) {
  auto cmd = inputParser.parseInput(query);
  try {
    inputParser.runCommand(cmd);
  } catch (const std::exception &e) {
    AppContext::GetInstance().getUserInterface()->writeLnErr(
        "Messages could not be performed.\n"
                              + std::string(typeid(e).name()) + "\n"
                              + std::string(e.what()));
  }
  AppContext::GetInstance().getUserInterface()->write("");
}

CLIController::CmdType CLIController::handleCommand(std::string_view command) {
  if (Utilities::compareString(command, "exit") == 0
      || Utilities::compareString(command, "e") == 0
      || Utilities::compareString(command, "quit") == 0
      || Utilities::compareString(command, "q") == 0) {
    return CmdType::exit;
  } else if (Utilities::compareString(command, "query start") == 0
      || Utilities::compareString(command, "qs") == 0) {
    return CmdType::queryModeStart;
  } else if (Utilities::compareString(command, "query end") == 0
      || Utilities::compareString(command, "qe") == 0) {
    return CmdType::queryModeEnd;
  } else if (Utilities::compareString(command, "help") == 0
      || Utilities::compareString(command, "h") == 0
      || Utilities::compareString(command, "?") == 0) {
    return CmdType::help;
  } else if (Utilities::compareString(command, "clear") == 0
      || Utilities::compareString(command, "c") == 0) {
    return CmdType::clear;
  } else if (command.substr(0, 10) == "run script") {
    scriptPath = command.substr(11);
    return CmdType::runScript;
  } else if (command == "list dbs" || command == "ldb") {
    return CmdType::listDBs;
  } else if (command.substr(0, 11) == "list tables") {
    dbName = Utilities::splitStringByDelimiter(command, " ")[1];
    return CmdType::listTables;
  } else if (command.substr(0, 4) == "ltbl") {
    dbName = Utilities::splitStringByDelimiter(command, " ")[1];
    return CmdType::listTables;
  } else if (command.substr(0, 8) == "download") {
    auto parts = Utilities::splitStringByDelimiter(command, " ");
    if (parts.size() == 4 && parts[2] == "to") {
      fileName = parts[1];
      savePath = parts[3];
      if (parts[0] == "downloadasync") {
        return CmdType::downloadAsync;
      }
      return CmdType::download;
    }
  } else if (command == "wait download") {
    return CmdType::waitDownload;
  }

  AppContext::GetInstance().getUserInterface()->writeLn(
      "Ignoring unknown command.");
  return CmdType::unknown;
}

void CLIController::printHelp() {
  AppContext::GetInstance().getUserInterface()->write("exit\t\texit/e/quit\n"
                   "query mode\tquery start/qs\n"
                   "query end\tquery end/qe\n"
                   "clear\t\tclear/c\n");
}

void CLIController::RunScript(std::string_view scriptPath) {
  auto path = std::string(scriptPath);
  std::ifstream input(path);
  auto scriptRunner = ScriptParser::GetInstance();
  if (!input.is_open()) {
    AppContext::GetInstance().getUserInterface()->writeLnErr(
        "Can't open script file.");
  }
  std::string line;
  try {
    while (std::getline(input, line)) {
      AppContext::GetInstance().getUserInterface()->writeLn("Running: " + line);
      if (!scriptRunner.runCommand(scriptRunner.parseInput(line))) {
        AppContext::GetInstance().getUserInterface()->writeLnErr(
            "Interrupting script execution");
        break;
      }
    }
    AppContext::GetInstance().getUserInterface()->writeLn(
        "Script finished successfully.");
  } catch (const std::exception &e) {
    AppContext::GetInstance().getUserInterface()->writeLnErr(
        "Messages could not be performed.\n"
                              + std::string(typeid(e).name()) + "\n"
                              + std::string(e.what()));
  }
  AppContext::GetInstance().getUserInterface()->write("");
}

void CLIController::receive(std::shared_ptr<Message> message) {
  if (auto msg = std::dynamic_pointer_cast<StdinMsg>(message)) {
    handleInput(msg->getData());
  } else if (auto msg = std::dynamic_pointer_cast<DownloadProgress>(message)) {
    std::string outMsg = "Download of '" + msg->getData().second + "' has ";
    switch (msg->getData().first) {
      case DownloadState::started:outMsg += "started.";
        break;
      case DownloadState::finished:outMsg += "finished.";
        break;
      case DownloadState::failed:outMsg += "failed.";
        break;
    }
    AppContext::GetInstance().getUserInterface()->writeLn(outMsg);
  }
}
void CLIController::handleInput(std::string_view input) {
  if (input.length() > 0) {
    if (AppContext::GetInstance().mode == AppContext::Mode::normal) {
      if (input[0] == ':') {
        handleQuery(input.substr(1));
      } else {
        switch (handleCommand(input)) {
          case CmdType::exit: {
            dispatch(new UIEnd());
            return;
          }
          case CmdType::waitDownload:
            AppContext::GetInstance().getUserInterface()->writeLn(
                "Waiting for downloads...");
            AppContext::GetInstance().getThreadPool()->wait(0);
            AppContext::GetInstance().getUserInterface()->writeLn(
                "Downloads finished...");
            break;
          case CmdType::download:dispatch(new Download(fileName, savePath));
            break;
          case CmdType::downloadAsync:
            dispatch(new DownloadNoBlock(fileName,
                                         savePath));
            break;
          case CmdType::queryModeStart:
            AppContext::GetInstance().mode = AppContext::Mode::query;
            AppContext::GetInstance().getUserInterface()->setMode(ConsoleIO::Mode::arrow);
            AppContext::GetInstance().getUserInterface()->writeLn("Query mode");
            AppContext::GetInstance().getUserInterface()->writeLn("**********");
            break;
          case CmdType::queryModeEnd:
            AppContext::GetInstance().mode = AppContext::Mode::normal;
            AppContext::GetInstance().getUserInterface()->setMode(ConsoleIO::Mode::simple);
            AppContext::GetInstance().getUserInterface()->writeLn("Normal mode");
            AppContext::GetInstance().getUserInterface()->writeLn(
                "\"***********\"");
            break;
          case CmdType::clear:AppContext::GetInstance().DBs.clear();
            break;
          case CmdType::help:printHelp();
            break;
          case CmdType::runScript:RunScript(scriptPath);
            break;
          case CmdType::listDBs:
            AppContext::GetInstance().getUserInterface()->writeLn(
                "List of DataBases:");
            for (const auto &db : AppContext::GetInstance().DBs) {
              AppContext::GetInstance().getUserInterface()->writeLn(db.first);
            }
            break;
          case CmdType::listTables:
            AppContext::GetInstance().getUserInterface()->writeLn(
                "List of tables in " + dbName + ": ");
            if (AppContext::GetInstance().DBs.find(dbName)
                == AppContext::GetInstance().DBs.end()) {
              AppContext::GetInstance().getUserInterface()->writeLnErr(
                  "DataBase not found.");
              break;
            }
            for (const auto
                  &val : AppContext::GetInstance().DBs[dbName]->getTables()) {
              AppContext::GetInstance().getUserInterface()->writeLn(val->getName());
            }
            break;
        }
      }
    } else {
      if (input[0] == ':') {
        switch (handleCommand(input.substr(1))) {
          case CmdType::queryModeEnd:
            AppContext::GetInstance().mode = AppContext::Mode::normal;
            AppContext::GetInstance().getUserInterface()->setMode(ConsoleIO::Mode::simple);
            AppContext::GetInstance().getUserInterface()->writeLn("Normal mode");
            AppContext::GetInstance().getUserInterface()->writeLn("***********");
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
