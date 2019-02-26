//
// Created by Petr Flajsingr on 2019-02-24.
//

#include <fstream>
#include <Config.h>
#include <Utilities.h>

#include "Config.h"

Config::Config(const std::string &path, bool autoCommit)
    : path(path), autoCommit(autoCommit) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::ofstream newFile(path);
    file.open(path);
  }
  load();
}

Config::~Config() {
  commit();
}

void Config::load() {
  std::ifstream file(path);
  std::string line;
  std::string curCat;
  while (std::getline(file, line)) {
    if (line.empty()) {
      continue;
    }
    if (line[0] == '[') {
      curCat = line.substr(1, line.length() - 2);
      categories[curCat] = {};
    } else {
      auto parts = Utilities::splitStringByDelimiter(line, "=");
      if (parts.size() != 2) {
        continue;
      }
      categories[curCat][parts[0]] = parts[1];
    }
  }
}

void Config::save() {
  std::ofstream file(path);
  for (const auto &cat : categories) {
    file << "[" << cat.first << "]\n";
    for (const auto &val : cat.second) {
      file << val.first << "=" << val.second << "\n";
    }
  }
}

void Config::commit() {
  if (!autoCommit) {
    save();
  }
}
