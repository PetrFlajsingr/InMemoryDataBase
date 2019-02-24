//
// Created by Petr Flajsingr on 2019-02-24.
//

#ifndef PROJECT_FILEDOWNLOADMANAGER_H
#define PROJECT_FILEDOWNLOADMANAGER_H

#include <string>
#include <ThreadPool.h>
#include "FileDownloadObserver.h"

class FileDownloadManager {
 public:
  FileDownloadManager();
  virtual ~FileDownloadManager();

  void enqueueDownload(const std::string &localFolder,
                       const std::string &url,
                       FileDownloadObserver &observer);
 private:
  ThreadPool threadPool;
};

#endif //PROJECT_FILEDOWNLOADMANAGER_H
