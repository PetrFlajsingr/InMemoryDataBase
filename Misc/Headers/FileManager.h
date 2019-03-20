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
  /**
   *
   * @param name relative path
   * @return folder on this.path+name
   */
  Folder getFolder(std::string_view name);
  /**
   * Create new folder.
   * @param name relative path
   * @return folder on this.path+name
   */
  Folder newFolder(std::string_view name);
  /**
   *
   * @param name relative path
   * @return true if this.path+name is folder, false otherwise
   */
  bool isFolder(std::string_view name);
  /**
   *
   * @param name relative path
   * @return file on this.path+name
   */
  File getFile(std::string_view name);
  /**
   * Create a new file.
   * @param name relative path
   * @return file on this.path+name
   */
  File newFile(std::string_view name);
  /**
   *
   * @param name relative path
   * @return true if this.path+name is a file, false otherwise
   */
  bool isFile(std::string_view name);

  const boost::filesystem::path &getPath() const;
};
/**
 * Basic access point to File/Folder classes.
 */
class FileManager {
 public:
  /**
   *
   * @param path absolute path
   * @return folder on path
   */
  Folder getFolder(std::string_view path);
  /**
   *
   * @param path absolute path
   * @return file on path
   */
  File getFile(std::string_view path);
};

#endif //PROJECT_FILEMANAGER_H
