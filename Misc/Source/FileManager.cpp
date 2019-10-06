//
// Created by Petr Flajsingr on 2019-02-26.
//

#include <FileManager.h>

#include "FileManager.h"

#include <utility>

FileException::FileException(const char *message) : IOException(message) {}
FileException::FileException(const std::string &message) : IOException(message) {}

Folder FileManager::getFolder(std::string_view path) {
  auto folderPath = boost::filesystem::path(std::string(path));
  if (!boost::filesystem::is_directory(folderPath)) {
    const auto err = std::string(path) + " is not a folder";
    throw FileException(err);
  }
  return Folder(folderPath);
}
File FileManager::getFile(std::string_view path) {
  auto folderPath = boost::filesystem::path(std::string(path));
  if (!boost::filesystem::is_regular_file(folderPath)) {
    const auto err = std::string(path) + " is not a file";
    throw FileException(err);
  }
  return File(folderPath);
}

const boost::filesystem::path &Folder::getPath() const { return path; }

Folder::Folder(boost::filesystem::path path) : path(std::move(path)) {}
Folder Folder::getFolder(std::string_view name) {
  std::string sPath = path.string();
  sPath.append(name);
  auto folderPath = boost::filesystem::path(sPath);
  if (!boost::filesystem::is_directory(folderPath)) {
    const auto err = sPath + " is not a folder";
    throw FileException(err);
  }
  return Folder(folderPath);
}
Folder Folder::newFolder(std::string_view name) {
  std::string sPath = path.string();
  sPath.append(name);
  auto folderPath = boost::filesystem::path(sPath);
  boost::filesystem::create_directory(folderPath);
  return Folder(folderPath);
}
File Folder::getFile(std::string_view name) {
  std::string sPath = path.string();
  sPath.append(name);
  auto filePath = boost::filesystem::path(sPath);
  if (!boost::filesystem::is_regular_file(filePath)) {
    const auto err = sPath + " is not a file";
    throw FileException(err);
  }
  return File(filePath);
}
File Folder::newFile(std::string_view name) {
  std::string sPath = path.string();
  sPath.append(name);
  auto filePath = boost::filesystem::path(sPath);
  std::ofstream os(sPath);
  os << "\n";
  return File(filePath);
}
bool Folder::isFolder(std::string_view name) {
  std::string sPath = path.string();
  sPath.append(name);
  auto folderPath = boost::filesystem::path(sPath);
  return boost::filesystem::is_directory(folderPath);
}
bool Folder::isFile(std::string_view name) {
  std::string sPath = path.string();
  sPath.append(name);
  auto filePath = boost::filesystem::path(sPath);
  return boost::filesystem::is_regular_file(filePath);
}

std::vector<std::string> Folder::getFileNames() const {
  namespace fs = boost::filesystem;
  std::vector<std::string> result;
  fs::directory_iterator end;

  for (fs::directory_iterator i(path); i != end; ++i) {
    const fs::path cp = (*i);
    result.emplace_back(cp.string());
  }
  return result;
}

File::File(boost::filesystem::path path) : path(std::move(path)) {}
std::ifstream File::inStream() { return std::ifstream(path.string()); }
std::ofstream File::ofStream() { return std::ofstream(path.string()); }
std::unique_ptr<DataProviders::BaseDataProvider> File::createProvider() { throw NotImplementedException(); }
std::unique_ptr<DataWriters::BaseDataWriter> File::createWriter() { throw NotImplementedException(); }
