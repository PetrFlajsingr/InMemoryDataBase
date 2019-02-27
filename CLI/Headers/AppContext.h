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
#include <Property.h>

class AppContext {
 public:

  std::unordered_map<std::string, std::shared_ptr<DataBase::MemoryDataBase>>
      DBs;
  enum class Mode {
    query, normal
  };
  Mode mode = Mode::normal;

  ReadOnlyProperty<std::shared_ptr<MessageManager>, AppContext> messageManager;
  ReadOnlyProperty<std::shared_ptr<ConsoleIO>, AppContext> ui;
  ReadOnlyProperty<std::shared_ptr<ResourceManager>,
                   AppContext> resourceManager;
  ReadOnlyProperty<std::shared_ptr<FileDownloadManager>,
                   AppContext> downloadManager;
  ReadOnlyProperty<std::shared_ptr<ThreadPool>, AppContext> threadPool;

  static AppContext &GetInstance();

 private:
  AppContext();
};

#endif //PROJECT_APPCONTEXT_H
