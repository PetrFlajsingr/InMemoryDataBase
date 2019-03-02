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
#include "../../Templates/Headers/Property.h"

class AppContext {
 public:

  std::unordered_map<std::string, std::shared_ptr<DataBase::MemoryDataBase>>
      DBs;
  enum class Mode {
    query, normal
  };
  Mode mode = Mode::normal;

  Property<std::shared_ptr<MessageManager>, AppContext, PropType::R>
      messageManager;
  Property<std::shared_ptr<ConsoleIO>, AppContext, PropType::R> ui;
  Property<std::shared_ptr<ResourceManager>,
           AppContext, PropType::R> resourceManager;
  Property<std::shared_ptr<FileDownloadManager>,
           AppContext,
           PropType::R> downloadManager;
  Property<std::shared_ptr<ThreadPool>, AppContext, PropType::R> threadPool;

  static AppContext &GetInstance();

 private:
  AppContext();
};

#endif //PROJECT_APPCONTEXT_H
