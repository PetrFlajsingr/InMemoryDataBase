//
// Created by Petr Flajsingr on 2019-02-23.
//

#ifndef PROJECT_APPCONTEXT_H
#define PROJECT_APPCONTEXT_H

#include <unordered_map>
#include <MemoryDataBase.h>
#include <Config.h>
#include <MessageManager.h>
#include "ConsoleIO.h"
#include <ThreadPool.h>
#include <FileDownloadManager.h>
#include <ResourceManager.h>

class AppContext {
 public:
  std::unordered_map<std::string, std::shared_ptr<DataBase::MemoryDataBase>>
      DBs;
  enum class Mode {
    query, normal
  };
  Mode mode = Mode::normal;

  std::shared_ptr<MessageManager> getMessageManager();
  std::shared_ptr<ConsoleIO> getUserInterface();
  std::shared_ptr<ResourceManager> getResourceManager();

  static AppContext &GetInstance();

 private:
  std::shared_ptr<MessageManager>
      messageManager = std::make_shared<MessageManager>();
  std::shared_ptr<ConsoleIO>
      userInterface = std::make_shared<ConsoleIO>(messageManager);
  std::shared_ptr<ThreadPool>
      threadPool = std::make_shared<ThreadPool>(Utilities::getCoreCount());
  std::shared_ptr<FileDownloadManager> dlManager =
      std::make_shared<FileDownloadManager>(messageManager, threadPool);
  std::shared_ptr<ResourceManager> resourceManager =
      std::make_shared<ResourceManager>();

  AppContext() = default;
};

#endif //PROJECT_APPCONTEXT_H
