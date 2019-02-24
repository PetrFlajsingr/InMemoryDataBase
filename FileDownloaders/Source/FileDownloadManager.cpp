//
// Created by Petr Flajsingr on 2019-02-24.
//

#include <Utilities.h>
#include <FileDownloadManager.h>
#include <FileDownloader.h>

#include "FileDownloadManager.h"

FileDownloadManager::FileDownloadManager()
    : threadPool(Utilities::getCoreCount()) {
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

void FileDownloadManager::enqueueDownload(const std::string &localFolder,
                                          const std::string &url,
                                          FileDownloadObserver &observer,
                                          bool blocking) {
  auto downloadTask = [=, &observer] {
    FileDownloader downloader(localFolder);
    downloader.addObserver(&observer);
    downloader.downloadFile(url);
  };
  if (!blocking) {
    threadPool.enqueue(downloadTask);
  } else {
    threadPool.enqueue(downloadTask).get();
  }
}

FileDownloadManager::~FileDownloadManager() {
  curl_global_cleanup();
}
