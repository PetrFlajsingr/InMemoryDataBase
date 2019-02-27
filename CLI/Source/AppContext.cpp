//
// Created by Petr Flajsingr on 2019-02-23.
//

#include <AppContext.h>

#include "AppContext.h"
AppContext &AppContext::GetInstance() {
  static AppContext instance;
  return instance;
}

AppContext::AppContext() {
  messageManager.value = std::make_shared<MessageManager>();
  ui.value = std::make_shared<ConsoleIO>(messageManager);
  threadPool.value = std::make_shared<ThreadPool>(Utilities::getCoreCount());
  downloadManager.value =
      std::make_shared<FileDownloadManager>(messageManager, threadPool);
  resourceManager.value = std::make_shared<ResourceManager>();

  messageManager.value->registerMsg<TaggedExecAsyncNotify>(threadPool.value.get());
  messageManager.value->registerMsg<TaggedExecAsync>(threadPool.value.get());
  messageManager.value->registerMsg<ExecAsyncNotify>(threadPool.value.get());
  messageManager.value->registerMsg<ExecAsync>(threadPool.value.get());
}

