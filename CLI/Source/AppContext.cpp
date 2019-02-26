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
