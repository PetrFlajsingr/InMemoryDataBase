//
// Created by Petr Flajsingr on 2019-02-24.
//

#include <FileDownloadManager.h>

#include <utility>

FileDownloadManager::FileDownloadManager(const std::shared_ptr<MessageManager> &commandManager,
                                         std::shared_ptr<ThreadPool> threadPool)
        : MessageSender(commandManager), threadPool(std::move(threadPool)) {
  commandManager->registerMsg<Download>(this);
  commandManager->registerMsg<DownloadNoBlock>(this);
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void FileDownloadManager::enqueueDownload(const std::string &localFolder, const std::string &url,
                                          FileDownloadObserver &observer, bool blocking) {
    auto download = [=, &observer] {
    FileDownloader downloader(localFolder);
    auto cntObs = CountObserver(this);
    downloader.addObserver(&cntObs);
    downloader.addObserver(&observer);
    downloader.downloadFile(url);
  };
  if (!blocking) {
      threadPool->enqueue(download);
  } else {
      download();
  }
}

FileDownloadManager::~FileDownloadManager() {
  if (auto tmp = commandManager.lock()) {
    tmp->unregister(this);
  }
  // curl_global_cleanup();
}

void FileDownloadManager::receive(std::shared_ptr<Message> message) {
  if (auto msg = std::dynamic_pointer_cast<DownloadNoBlock>(message)) {
    auto dlLambda = [=] {
      downloadTask(msg);
    };
    dispatch(new TaggedExecAsync(dlLambda, 0));
  } else if (auto msg = std::dynamic_pointer_cast<Download>(message)) {
    downloadTask(msg);
  }
}
void FileDownloadManager::unfinished() {
  unfinishedFileCount++;
}
void FileDownloadManager::finished() {
  unfinishedFileCount--;
}

FileDownloadManager::CountObserver::CountObserver(FileDownloadManager *parent) : parent(parent) {}
void FileDownloadManager::CountObserver::onDownloadStarted(std::string_view) {
  parent->unfinished();
}
void FileDownloadManager::CountObserver::onDownloadFailed(std::string_view, std::string_view) {
  parent->finished();
}
void FileDownloadManager::CountObserver::onDownloadFinished(std::string_view, std::string_view) {
  parent->finished();
}

FileDownloadManager::Dispatcher::Dispatcher(const std::shared_ptr<MessageManager> &commandManager)
    : MessageSender(commandManager) {}
void FileDownloadManager::Dispatcher::onDownloadStarted(std::string_view fileName) {
  dispatch(new DownloadProgress(DownloadState::started, std::string(fileName)));
}
void FileDownloadManager::Dispatcher::onDownloadFailed(std::string_view fileName,
                                                       std::string_view) {
  dispatch(new DownloadProgress(DownloadState::failed, std::string(fileName)));
}
void FileDownloadManager::Dispatcher::onDownloadFinished(std::string_view fileName,
                                                         std::string_view) {
  dispatch(new DownloadProgress(DownloadState::finished, std::string(fileName)));
}

