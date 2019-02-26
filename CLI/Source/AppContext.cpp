//
// Created by Petr Flajsingr on 2019-02-23.
//

#include <AppContext.h>

#include "AppContext.h"
AppContext &AppContext::GetInstance() {
  static AppContext instance;
  return instance;
}
std::shared_ptr<MessageManager> AppContext::getMessageManager() {
  return messageManager;
}
std::shared_ptr<ConsoleIO> AppContext::getUserInterface() {
  return userInterface;
}
std::shared_ptr<ResourceManager> AppContext::getResourceManager() {
  return resourceManager;
}
std::shared_ptr<FileDownloadManager> AppContext::getDownloadManager() {
  return dlManager;
}
std::shared_ptr<ThreadPool> AppContext::getThreadPool() {
  return threadPool;
}
AppContext::AppContext() {
  messageManager->registerMessage<TaggedExecAsyncNotify>(threadPool.get());
  messageManager->registerMessage<TaggedExecAsync>(threadPool.get());
  messageManager->registerMessage<ExecAsyncNotify>(threadPool.get());
  messageManager->registerMessage<ExecAsync>(threadPool.get());
}
