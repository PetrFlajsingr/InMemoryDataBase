//
// Created by Petr Flajsingr on 2019-02-25.
//

#ifndef PROJECT_COMMANDRECEIVER_H
#define PROJECT_COMMANDRECEIVER_H

#include "Message.h"
#include <string>
#include <memory>

/**
 * Listener of messages.
 * Receive method is called when desired message type is sent.
 */
class MessageReceiver {
 private:
  virtual void receive(std::shared_ptr<Message> message) = 0;
  friend class MessageManager;
};

#endif //PROJECT_COMMANDRECEIVER_H
