//
// Created by Petr Flajsingr on 2019-02-24.
//

#include <Utilities.h>
#include <FileDownloadManager.h>
#include <FileDownloader.h>

FileDownloadManager::FileDownloadManager(const std::shared_ptr<MessageManager> &commandManager,
                                         const std::shared_ptr<ThreadPool> &threadPool)
    : MessageSender(commandManager), threadPool(threadPool) {
  commandManager->registerMessage<Download>(this);
  commandManager->registerMessage<DownloadNoBlock>(this);
  curl_global_init(CURL_GLOBAL_DEFAULT);
}

void FileDownloadManager::enqueueDownload(const std::string &localFolder,
                                          const std::string &url,
                                          FileDownloadObserver &observer,
                                          bool blocking) {
  auto downloadTask = [=, &observer] {
    FileDownloader downloader(localFolder);
    auto cntObs = CountObserver(this);
    downloader.addObserver(&cntObs);
    downloader.addObserver(&observer);
    downloader.downloadFile(url);
  };
  if (!blocking) {
    threadPool->enqueue(downloadTask);
  } else {
    threadPool->enqueue(downloadTask).get();
  }
}

FileDownloadManager::~FileDownloadManager() {
  if (auto tmp = commandManager.lock()) {
    tmp->unregister(this);
  }
  curl_global_cleanup();
}
void FileDownloadManager::waitForDownloads() {
  std::unique_lock lck{mutex};
  downloadsDone.wait(lck, [&] { return unfinishedFileCount == 0; });
}

void FileDownloadManager::receive(std::shared_ptr<Message> message) {
  auto downloadTask = [=](const std::shared_ptr<Download> &msg) {
    FileDownloader downloader(msg->getData().second);
    auto cntObs = CountObserver(this);
    downloader.addObserver(&cntObs);
    auto dispatchObserver = Dispatcher(commandManager.lock());
    downloader.addObserver(&dispatchObserver);
    downloader.downloadFile(msg->getData().first);
  };
  if (auto msg = std::dynamic_pointer_cast<DownloadNoBlock>(message)) {
    auto dlLambda = [=] {
      downloadTask(msg);
    };
    threadPool->enqueue(dlLambda);
  } else if (auto msg = std::dynamic_pointer_cast<Download>(message)) {
    downloadTask(msg);
  }
}
void FileDownloadManager::unfinished() {
  unfinishedFileCount++;
}
void FileDownloadManager::finished() {
  unfinishedFileCount--;
  if (unfinishedFileCount == 0) {
    downloadsDone.notify_all();
  }
}

FileDownloadManager::CountObserver::CountObserver(FileDownloadManager *parent)
    : parent(parent) {}
void FileDownloadManager::CountObserver::onDownloadStarted(std::string_view) {
  parent->unfinished();
}
void FileDownloadManager::CountObserver::onDownloadFailed(std::string_view,
                                                          std::string_view) {
  parent->finished();
}
void FileDownloadManager::CountObserver::onDownloadFinished(std::string_view,
                                                            std::string_view) {
  parent->finished();
}

FileDownloadManager::Dispatcher::Dispatcher(const std::shared_ptr<MessageManager> &commandManager)
    : MessageSender(commandManager) {}
void FileDownloadManager::Dispatcher::onDownloadStarted(std::string_view fileName) {
  dispatch(new DownloadProgress(DownloadState::started, std::string(fileName)));
}
void FileDownloadManager::Dispatcher::onDownloadFailed(std::string_view fileName,
                                                       std::string_view errorMessage) {
  dispatch(new DownloadProgress(DownloadState::failed, std::string(fileName)));
}
void FileDownloadManager::Dispatcher::onDownloadFinished(std::string_view fileName,
                                                         std::string_view filePath) {
  dispatch(new DownloadProgress(DownloadState::finished,
                                std::string(fileName)));
}

