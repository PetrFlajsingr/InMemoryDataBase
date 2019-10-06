//
// Created by Petr Flajsingr on 2019-02-23.
//

#ifndef PROJECT_APPCONTEXT_H
#define PROJECT_APPCONTEXT_H

#include "ConsoleIO.h"
#include <Config.h>
#include <FileDownloadManager.h>
#include <MemoryDataBase.h>
#include <MessageManager.h>
#include <Property.h>
#include <ResourceManager.h>
#include <ThreadPool.h>
#include <memory>
#include <string>
#include <unordered_map>

/**
 * Global data and managers.
 */
class AppContext {
public:
  std::unordered_map<std::string, std::shared_ptr<DataBase::MemoryDataBase>> DBs;
  /**
   * CLI input mode.
   */
  enum class Mode { query, normal };
  Mode mode = Mode::normal;

  Property<std::shared_ptr<MessageManager>, AppContext, PropType::R> messageManager;
  Property<std::shared_ptr<ConsoleIO>, AppContext, PropType::R> ui;
  Property<std::shared_ptr<ResourceManager>, AppContext, PropType::R> resourceManager;
  Property<std::shared_ptr<FileDownloadManager>, AppContext, PropType::R> downloadManager;
  Property<std::shared_ptr<ThreadPool>, AppContext, PropType::R> threadPool;

  static AppContext &GetInstance();

private:
  AppContext();
};

#endif // PROJECT_APPCONTEXT_H
