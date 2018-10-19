//
// Created by Petr Flajsingr on 19/10/2018.
//

#ifndef FILEDOWNLOADERS_HEADERS_BASEFILEDOWNLOADER_H_
#define FILEDOWNLOADERS_HEADERS_BASEFILEDOWNLOADER_H_

#include <vector>
#include <string>
#include "Exceptions.h"
#include "FileDownloadObserver.h"

class BaseFileDownloader {
 private:
  std::vector<FileDownloadObserver*> observers;

  void notifyDownloadStarted(const std::string &fileName) {
    for (auto observer : observers) {
      observer->onDownloadStarted(fileName);
    }
  }

  void notifyDownloadFailed(const std::string &fileName,
                            const std::string &errorMessage) {
    for (auto observer : observers) {
      observer->onDownloadFailed(fileName, errorMessage);
    }
  }

  void notifyDownloadFinished(const std::string &fileName,
                              const std::string &filePath) {
    for (auto observer : observers) {
      observer->onDownloadFinished(fileName, filePath);
    }
  }

 protected:
  std::vector<std::string> availableFiles;

 public:
  virtual ~BaseFileDownloader() = default;

  virtual std::vector<std::string> getAvailableFiles() {
    return availableFiles;
  }

  virtual void downloadFile(const std::string &fileName) {
    auto result = std::find(availableFiles.begin(),
        availableFiles.end(),
        fileName);

    if (result == availableFiles.end()) {
      throw InvalidArgumentException(
          ("The file " + fileName + " is not available").c_str());
    }

    downloadFile(result - availableFiles.begin());
  }

  virtual void downloadFile(size_t fileIndex) = 0;

  virtual void addObserver(FileDownloadObserver* observer) {
    observers.push_back(observer);
  }
};

#endif  // FILEDOWNLOADERS_HEADERS_BASEFILEDOWNLOADER_H_
