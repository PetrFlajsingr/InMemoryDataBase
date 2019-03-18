//
// Created by Petr Flajsingr on 2019-02-26.
//

#ifndef PROJECT_FILEMANAGER_H
#define PROJECT_FILEMANAGER_H

#include <string>
#include <BaseDataProvider.h>
#include <BaseDataWriter.h>
#include <boost/filesystem.hpp>
#include <Exceptions.h>

class FileException : public IOException {
 public:
  explicit FileException(const char *message);
  explicit FileException(const std::string &message);
};
/**
 * Represents a file. Provides methods to open said file.
 */
class File {
 private:
  boost::filesystem::path path;
 public:
  explicit File(const boost::filesystem::path &path);
  DataProviders::BaseDataProvider createProvider();
  DataWriters::BaseDataWriter createWriter();
  std::ifstream inStream();
  std::ofstream ofStream();
};
/**
 * Represents a folder. Provides methods to move in file structure.
 */
class Folder {
 private:
  boost::filesystem::path path;
 public:
  explicit Folder(const boost::filesystem::path &path);
  Folder getFolder(std::string_view name);
  Folder newFolder(std::string_view name);
  bool isFolder(std::string_view name);
  File getFile(std::string_view name);
  File newFile(std::string_view name);
  bool isFile(std::string_view name);

  const boost::filesystem::path &getPath() const;
};
/**
 * Basic access point to File/Folder classes.
 */
class FileManager {
 public:
  Folder getFolder(std::string_view path);
  File getFile(std::string_view path);
};

#endif //PROJECT_FILEMANAGER_H
