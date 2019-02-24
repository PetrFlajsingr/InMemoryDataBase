//
// Created by Petr Flajsingr on 2019-02-23.
//

#include <ScriptParser.h>
#include <string>

#include "ScriptParser.h"
#include <ConsoleIO.h>
#include <AppContext.h>
#include <CsvReader.h>
#include <XlsReader.h>
#include <CsvWriter.h>
#include <XlsxWriter.h>

ScriptParser::Command ScriptParser::parseInput(std::string_view input) {
  auto tokens = tokenize(input);
  if (tokens.size() == 3) {
    if (tokens[1] == "database") {
      if (tokens[0] == "create") {
        dbName = tokens[2];
        return Command::createDB;
      } else if (tokens[0] == "remove") {
        dbName = tokens[2];
        return Command::removeDB;
      } else {
        return Command::unknown;
      }
    } else {
      return Command::unknown;
    }
  } else if (tokens.size() == 6) {
    if (tokens[0] == "save" && tokens[2] == "as"
        && (tokens[3] == "csv" || tokens[3] == "xlsx")
        && (tokens[5] == "normal" || tokens[5] == "divided")) {
      return Command::save;
    } else {
      return Command::unknown;
    }
  } else if (tokens.size() > 9 && tokens[0] == "load") {
    if (tokens[2] == "file" && tokens[4] == "to"
        && tokens[6] == "as" && tokens[8] == "types") {
      if (tokens[1] == "csv") {
        fileType = FileTypes::csv;
      } else if (tokens[1] == "xlsx") {
        fileType = FileTypes::xlsx;
      } else {
        return Command::unknown;
      }
      filePath = tokens[3];
      dbName = tokens[5];
      dataSetName = tokens[7];
      valueTypes.clear();
      for (auto it = tokens.begin() + 9; it != tokens.end(); ++it) {
        if (*it == "string") {
          valueTypes.emplace_back(ValueType::String);
        } else if (*it == "integer") {
          valueTypes.emplace_back(ValueType::Integer);
        } else if (*it == "double") {
          valueTypes.emplace_back(ValueType::Double);
        } else if (*it == "currency") {
          valueTypes.emplace_back(ValueType::Currency);
        } else if (*it == "datetime") {
          valueTypes.emplace_back(ValueType::DateTime);
        } else {
          return Command::unknown;
        }
      }
      return Command::load;
    } else {
      return Command::unknown;
    }
  } else if (tokens.size() >= 10 && tokens[0] == "query") {
    if (tokens[3] == "as" && tokens[5] == "save" && tokens[6] == "as") {
      query = tokens[2];
      dataSetName = tokens[4];
      if (tokens[7] == "csv") {
        fileType = FileTypes::csv;
      } else if (tokens[7] == "xlsx") {
        fileType = FileTypes::xlsx;
      } else {
        return Command::unknown;
      }
      dbName = tokens[1];
      filePath = tokens[8];
      saveMode = SaveMode::normal;
      return Command::query;
    } else {
      return Command::unknown;
    }
  } else if (tokens.size() == 7 && tokens[0] == "append") {
    if (tokens[1] == "csv") {
      fileType = FileTypes::csv;
    } else if (tokens[1] == "xlsx") {
      fileType = FileTypes::xlsx;
    } else {
      return Command::unknown;
    }
    if (tokens[2] == "file" && tokens[4] == "to") {
      filePath = tokens[3];
      dbName = tokens[5];
      dataSetName = tokens[6];
    } else {
      return Command::unknown;
    }
    return Command::append;
  }
  return Command::unknown;
}

std::vector<std::string> ScriptParser::tokenize(std::string_view str) {
  std::vector<std::string> result;
  std::string part;
  bool noSpaces = false;
  for (const auto val : str) {
    if (noSpaces) {
      if (val == '\'') {
        noSpaces = false;
      } else {
        part += val;
      }
      continue;
    }
    switch (val) {
      case ' ':result.emplace_back(part);
        part.clear();
        break;
      case '\'':noSpaces = true;
        break;
      default:part += val;
        break;
    }
  }
  result.emplace_back(part);
  return result;
}
bool ScriptParser::runCommand(ScriptParser::Command command) {
  switch (command) {
    case ScriptParser::Command::createDB:
      if (AppContext::GetInstance().DBs.find(dbName)
          == AppContext::GetInstance().DBs.end()) {
        AppContext::GetInstance().DBs[dbName] =
            std::make_shared<DataBase::MemoryDataBase>(dbName);
      } else {
        ConsoleIO::writeLnErr("DataBase already exists.");
        return false;
      }
      break;
    case ScriptParser::Command::removeDB:
      AppContext::GetInstance().DBs.erase(dbName);
      break;
    case ScriptParser::Command::load: {
      if (AppContext::GetInstance().DBs.find(dbName)
          == AppContext::GetInstance().DBs.end()) {
        ConsoleIO::writeLnErr("DataBase doesn't exist.");
        return;
      }
      DataProviders::BaseDataProvider *provider;
      switch (fileType) {
        case ScriptParser::FileTypes::csv:
          provider =
              new DataProviders::CsvReader(filePath);
          break;
        case ScriptParser::FileTypes::xlsx:
          provider =
              new DataProviders::XlsReader(filePath);
          break;
      }
      auto ds =
          std::make_shared<DataSets::MemoryDataSet>(dataSetName);
      ds->open(*provider, valueTypes);
      AppContext::GetInstance().DBs[dbName]->addTable(ds);
      delete provider;
    }
      break;
    case ScriptParser::Command::append:
      if (AppContext::GetInstance().DBs.find(dbName)
          == AppContext::GetInstance().DBs.end()) {
        ConsoleIO::writeLnErr("DataBase doesn't exist.");
        return false;
      }
      {
        DataProviders::BaseDataProvider *provider;
        switch (fileType) {
          case ScriptParser::FileTypes::csv:
            provider =
                new DataProviders::CsvReader(
                    filePath);
            break;
          case ScriptParser::FileTypes::xlsx:
            provider =
                new DataProviders::XlsReader(
                    filePath);
            break;
        }
        auto ds =
            AppContext::GetInstance().DBs[dbName]->tableByName(dataSetName);
        ds->dataSet->append(*provider);
      }
      break;
    case ScriptParser::Command::removeTab:
      if (AppContext::GetInstance().DBs.find(dbName)
          == AppContext::GetInstance().DBs.end()) {
        ConsoleIO::writeLnErr("DataBase doesn't exist.");
        return false;
      }
      AppContext::GetInstance().DBs[dbName]->removeTable(dataSetName);
      break;
    case ScriptParser::Command::query:
      if (AppContext::GetInstance().DBs.find(dbName)
          == AppContext::GetInstance().DBs.end()) {
        ConsoleIO::writeLnErr("DataBase doesn't exist.");
        return false;
      }
      {
        auto result = AppContext::GetInstance().DBs[dbName]->execSimpleQuery(
            query,
            false,
            dataSetName);
        DataWriters::BaseDataWriter *writer;
        switch (fileType) {
          case ScriptParser::FileTypes::csv:
            writer = new DataWriters::CsvWriter(filePath);
            break;
          case ScriptParser::FileTypes::xlsx:
            writer = new DataWriters::XlsxWriter(filePath);
            break;
        }
        writer->writeHeader(result->dataSet->getFieldNames());
        auto fields = result->dataSet->getFields();
        while (result->dataSet->next()) {
          std::vector<std::string> record;
          std::transform(fields.begin(),
                         fields.end(),
                         std::back_inserter(record),
                         [](const DataSets::BaseField *field) {
                           return std::string(field->getAsString());
                         });
          writer->writeRecord(record);
        }
        delete writer;
      }
      break;
    case ScriptParser::Command::save:break;
    case ScriptParser::Command::unknown:ConsoleIO::writeLnErr("Unknown command.");
      break;
  }
  return true;
}
ScriptParser &ScriptParser::GetInstance() {
  static ScriptParser instance;
  return instance;
}
