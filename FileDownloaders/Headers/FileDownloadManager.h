//
// Created by Petr Flajsingr on 2019-02-24.
//

#ifndef PROJECT_FILEDOWNLOADMANAGER_H
#define PROJECT_FILEDOWNLOADMANAGER_H

#include <string>
#include <ThreadPool.h>
#include "FileDownloadObserver.h"
#include <MessageSender.h>
#include <MessageReceiver.h>

class FileDownloadManager : public MessageReceiver, public MessageSender {
 public:
  FileDownloadManager(const std::shared_ptr<MessageManager> &commandManager,
                      const std::shared_ptr<ThreadPool> &threadPool);
  virtual ~FileDownloadManager();

  void enqueueDownload(const std::string &localFolder,
                       const std::string &url,
                       FileDownloadObserver &observer,
                       bool blocking = false);

  void waitForDownloads();
 private:
  void receive(std::shared_ptr<Message> message) override;
  std::shared_ptr<ThreadPool> threadPool;

  std::atomic<int> unfinishedFileCount = 0;

  std::condition_variable downloadsDone;
  std::mutex mutex;

  class CountObserver : public FileDownloadObserver {
   public:
    explicit CountObserver(FileDownloadManager *parent);
    void onDownloadStarted(std::string_view) override;
    void onDownloadFailed(std::string_view,
                          std::string_view) override;
    void onDownloadFinished(std::string_view,
                            std::string_view) override;
   private:
    FileDownloadManager *parent;
  };

  class Dispatcher : public FileDownloadObserver, public MessageSender {
   public:
    explicit Dispatcher(const std::shared_ptr<MessageManager> &commandManager);
    void onDownloadStarted(std::string_view fileName) override;
    void onDownloadFailed(std::string_view fileName,
                          std::string_view errorMessage) override;
    void onDownloadFinished(std::string_view fileName,
                            std::string_view filePath) override;
  };
};

#endif //PROJECT_FILEDOWNLOADMANAGER_H
