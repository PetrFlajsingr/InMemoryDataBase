//
// Created by Petr Flajsingr on 2019-04-09.
//
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <CsvWriter.h>
#include <FileManager.h>
#include <Logger.h>
#include <CsvReader.h>
#include "ARES/ZakladniUdaje.h"

namespace pt = boost::property_tree;
using namespace std::string_literals;
using namespace std::chrono_literals;

using MyLogger = Logger<true>;


std::string indent(int level) {
  const auto indentThingy = "  ";
  std::string result;
  for (int i = 0; i < level; ++i) {
    result += indentThingy;
  }
  return result;
}

std::vector<std::pair<std::string, int>> prepareHeaderInd(const pt::ptree &pt, int level = 0) {
  std::vector<std::pair<std::string, int>> result;
  if (!pt.empty()) {
    for (auto &val : pt) {
      if (val.second.empty()) {
        result.emplace_back(val.first, level);
      }

      auto innerResult = prepareHeaderInd(val.second, level + 1);
      result.insert(result.end(), innerResult.begin(), innerResult.end());
    }
  }
  return result;
}

std::vector<std::string> prepareHeader(const pt::ptree &pt, int level = 0) {
  std::vector<std::string> result;
  if (!pt.empty()) {
    for (auto &val : pt) {
      if (val.second.empty()) {
        result.emplace_back(val.first);
      }

      auto innerResult = prepareHeader(val.second, level + 1);
      result.insert(result.end(), innerResult.begin(), innerResult.end());
    }
  }
  return result;
}

std::vector<std::string> readData(const pt::ptree &pt, int level = 0) {
  std::vector<std::string> result;
  if (pt.empty()) {
    result.emplace_back(pt.data());
  } else {

    for (auto &val: pt) {
      auto innerResult = readData(val.second, level + 1);
      result.insert(result.end(), innerResult.begin(), innerResult.end());
    }
  }
  return result;
}

int main() {

  const auto fileName = "/Users/petr/Desktop/MUNI.nosync/ares/VYSTUP/DATA/28551923.xml";
  std::ifstream ifstream(fileName);
  /*std::string str((std::istreambuf_iterator<char>(ifstream)),
                  std::istreambuf_iterator<char>());
  std::cout << str <<std::endl;
  std::cout << "________________" << std::endl;*/
  pt::ptree tree;

  pt::read_xml(ifstream, tree);

  ZakladniUdaje udaje(tree);

  for(auto &ffs : udaje.toVector()) {
    std::cout << ffs << std::endl;
  }

  /*const std::array<std::string, 1> icos{"28551923"};

  for (const auto &val : icos) {
    const auto fileName = "/Users/petr/Desktop/MUNI.nosync/ares/VYSTUP/DATA/"s + val + ".xml";
    std::ifstream ifstream(fileName);
    if (!ifstream.is_open()) {
      MyLogger::GetInstance().log<LogLevel::Status>("File not found: "s + fileName);
      continue;
    }
    pt::ptree tree;
    pt::read_xml(ifstream, tree);
    auto header = prepareHeaderInd(tree);
    const auto data = readData(tree);

    for (auto &val : header) {
      if (val.first.length() > 4) {
        val.first = val.first.substr(4);
      }
    }

    std::ofstream ofstream("/Users/petr/Desktop/ares_txts/"s + val + ".txt");
    for (int i = 0; i < header.size(); ++i) {
      ofstream << indent(header[i].second) + header[i].first + ": " + data[i] + "\n";
    }
    ofstream.close();

    MyLogger::GetInstance().log<LogLevel::Status>("File transformed: "s + fileName);
  }*/

  /*pt::ptree tree;

  DataWriters::CsvWriter writer("/Users/petr/Desktop/xmlTestOut.csv", ",");

  pt::read_xml("/Users/petr/Desktop/test.xml", tree);

  header = prepareHeader(tree);
  writer.writeHeader(header);

  FileManager fileManager;

  auto mainDir = fileManager.getFolder("/Users/petr/Desktop/MUNI.nosync/ares/VYSTUP/DATA");
  auto fileNames = mainDir.getFileNames();
  std::sort(fileNames.begin(), fileNames.end());

  for (const auto &fileName: fileNames) {
    std::cout << "File: " << fileName << std::endl;
    cnt = 0;
    pt::read_xml(fileName, tree);
    auto data = createRow(tree);
    for (; cnt < header.size(); ++cnt) {
      data.emplace_back("null");
    }
    writer.writeRecord(data);
    std::cout << " done" << std::endl;
  }*/

  return 0;
}