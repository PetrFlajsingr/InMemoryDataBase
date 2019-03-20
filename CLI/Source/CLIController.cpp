//
// Created by Petr Flajsingr on 2019-02-23.
//

#include <CLIController.h>
#include <CsvReader.h>
#include <XlsxIOReader.h>
#include <XlsxIOWriter.h>

CLIController::CLIController()
    : MessageSender(AppContext::GetInstance().messageManager) {}

void CLIController::runApp() {
  auto tmp = ctx.messageManager.operator->();
  ctx.messageManager->registerMsg<StdinMsg>(
      this);
  ctx.messageManager->registerMsg<
      DownloadProgress>(
      this);

  ctx.mode = AppContext::Mode::normal;

  ctx.ui->writeLn("Normal mode");
  ctx.ui->writeLn("___________");
  ctx.ui->listenInput();
}

void CLIController::handleQuery(std::string_view query) {
  auto cmd = inputParser.parseInput(query);
  try {
    inputParser.runCommand(cmd);
  } catch (const std::exception &e) {
    ctx.ui->writeLnErr(
        "Command could not be performed.\n"
                              + std::string(typeid(e).name()) + "\n"
                              + std::string(e.what()));
  }
  ctx.ui->write("");
}

CLIController::CmdType CLIController::handleCommand(std::string_view input) {
  auto command = ScriptParser::ReplaceResources(std::string(input));
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

  ctx.ui->writeLn(
      "Ignoring unknown command.");
  return CmdType::unknown;
}

void CLIController::printHelp() {
  ctx.ui->write("exit\t\texit/e/quit\n"
                   "query mode\tquery start/qs\n"
                   "query end\tquery end/qe\n"
                   "clear\t\tclear/c\n");
}

void CLIController::RunScript(std::string_view scriptPath) {
  auto &context = AppContext::GetInstance();
  auto path = std::string(scriptPath);
  std::ifstream input(path);
  auto scriptRunner = ScriptParser::GetInstance();
  if (!input.is_open()) {
    context.ui->writeLnErr(
        "Can't open script file.");
  }
  std::string line;
  try {
    while (std::getline(input, line)) {
      context.ui->writeLn("Running: " + line);
      if (!scriptRunner.runCommand(scriptRunner.parseInput(line))) {
        context.ui->writeLnErr(
            "Interrupting script execution");
        break;
      }
    }
    context.ui->writeLn(
        "Script finished successfully.");
  } catch (const std::exception &e) {
    context.ui->writeLnErr(
        "Messages could not be performed.\n"
                              + std::string(typeid(e).name()) + "\n"
                              + std::string(e.what()));
  }
  context.ui->write("");
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
    ctx.ui->writeLn(outMsg);
  }
}
void CLIController::handleInput(std::string_view input) {
  if (input.length() > 0) {
    if (ctx.mode == AppContext::Mode::normal) {
      if (input[0] == ':') {
        handleQuery(input.substr(1));
      } else {
        switch (handleCommand(input)) {
          case CmdType::exit: {
            dispatch(new UIEnd());
            return;
          }
          case CmdType::waitDownload:
            ctx.ui->writeLn(
                "Waiting for downloads...");
            ctx.threadPool->wait(0);
            ctx.ui->writeLn(
                "Downloads finished...");
            break;
          case CmdType::download:dispatch(new Download(fileName, savePath));
            break;
          case CmdType::downloadAsync:
            dispatch(new DownloadNoBlock(fileName,
                                         savePath));
            break;
          case CmdType::queryModeStart:ctx.mode = AppContext::Mode::query;
            ctx.ui->setMode(ConsoleIO::Mode::arrow);
            ctx.ui->writeLn("Query mode");
            ctx.ui->writeLn("**********");
            break;
          case CmdType::queryModeEnd:ctx.mode = AppContext::Mode::normal;
            ctx.ui->setMode(ConsoleIO::Mode::simple);
            ctx.ui->writeLn("Normal mode");
            ctx.ui->writeLn(
                "\"***********\"");
            break;
          case CmdType::clear:ctx.DBs.clear();
            break;
          case CmdType::help:printHelp();
            break;
          case CmdType::runScript:RunScript(scriptPath);
            break;
          case CmdType::listDBs:
            ctx.ui->writeLn(
                "List of DataBases:");
            for (const auto &db : ctx.DBs) {
              ctx.ui->writeLn(db.first);
            }
            break;
          case CmdType::listTables:
            ctx.ui->writeLn(
                "List of tables in " + dbName + ": ");
            if (ctx.DBs.find(dbName)
                == ctx.DBs.end()) {
              ctx.ui->writeLnErr(
                  "DataBase not found.");
              break;
            }
            for (const auto
                  &val : ctx.DBs[dbName]->getTables()) {
              ctx.ui->writeLn(val->getName());
            }
            break;
        }
      }
    } else {
      if (input[0] == ':') {
        switch (handleCommand(input.substr(1))) {
          case CmdType::queryModeEnd:ctx.mode = AppContext::Mode::normal;
            ctx.ui->setMode(ConsoleIO::Mode::simple);
            ctx.ui->writeLn("Normal mode");
            ctx.ui->writeLn("***********");
            break;
          case CmdType::clear:ctx.DBs.clear();
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
