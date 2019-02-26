//
// Created by Petr Flajsingr on 2019-02-25.
//

#ifndef PROJECT_COMMANDMANAGER_H
#define PROJECT_COMMANDMANAGER_H

#include <Message.h>
#include <Exceptions.h>
#include <unordered_map>
#include "MessageReceiver.h"
#include <vector>
#include <iostream>

class MessageManager {
 public:
  template<typename T>
  void registerMessage(MessageReceiver *receiver) {
    static_assert(std::is_base_of<Message, T>{});
    commands[typeid(T).name()].emplace_back(receiver);
  }

  template<typename T>
  void unregisterCommand(MessageReceiver *receiver) {
    auto &vec = commands[typeid(T).name()];
    if (auto it = std::find(vec.begin(), vec.end(), receiver); it
        != vec.end()) {
      vec.erase(it);
    }
  }

  void unregister(MessageReceiver *receiver) {
    std::for_each(commands.begin(), commands.end(),
                  [receiver](std::pair<const std::string,
                                       std::vector<MessageReceiver *>> &record) {
                    if (auto it = std::find(record.second.begin(),
                                            record.second.end(),
                                            receiver); it
                        != record.second.end()) {
                      record.second.erase(it);
                    }
                  });
  }

  template<typename T>
  void dispatch(std::shared_ptr<T> command) {
    static_assert(std::is_base_of<Message, T>{});
    auto receivers = commands[typeid(*command.get()).name()];
    std::for_each(receivers.begin(),
                  receivers.end(),
                  [&command](MessageReceiver *receiver) {
                    receiver->receive(command);
                  });
  }
 private:
  std::unordered_map<std::string, std::vector<MessageReceiver *>> commands;
};

#endif //PROJECT_COMMANDMANAGER_H
