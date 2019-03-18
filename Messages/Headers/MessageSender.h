//
// Created by Petr Flajsingr on 2019-02-25.
//

#ifndef PROJECT_COMMANDSENDER_H
#define PROJECT_COMMANDSENDER_H

#include "Message.h"
#include "MessageManager.h"
#include <memory>

/**
 * Allows usage of dispatch method to send messages to all Manager listeners.
 */
class MessageSender {
 public:
  explicit MessageSender(const std::shared_ptr<MessageManager> &commandManager)
      : commandManager(commandManager) {}
 protected:
  void dispatch(Message *message) {
    if (auto tmp = commandManager.lock()) {
      tmp->dispatch(std::shared_ptr<Message>(message));
    }
  }
  std::weak_ptr<MessageManager> commandManager;
};

#endif //PROJECT_COMMANDSENDER_H
