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
