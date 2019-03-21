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
#include <typeindex>

/**
 * This class takes care of sending messages to listeners who registered them.
 * It allows to unregister as well.
 * It dispatches messages only to those who registered the specific type.
 */
class MessageManager {
 public:
  /**
   * Register receiver for certain message.
   * @tparam T type of message to register
   * @param receiver listener
   */
  template<typename T>
  void registerMsg(MessageReceiver *receiver) {
    static_assert(std::is_base_of<Message, T>{});
    commands[typeid(T)].emplace_back(receiver);
  }
  /**
   * Unregister receiver from gettin certain messages.
   * @tparam T type of message to unregister
   * @param receiver listener
   */
  template<typename T>
  void unregisterMsg(MessageReceiver *receiver) {
    auto &vec = commands[typeid(T)];
    if (auto it = std::find(vec.begin(), vec.end(), receiver); it != vec.end()) {
      vec.erase(it);
    }
  }
  /**
   * Unregister receiver from all messages
   * @param receiver listener
   */
  void unregister(MessageReceiver *receiver) {
    std::for_each(commands.begin(), commands.end(),
                  [receiver](std::pair<const std::type_index, std::vector<MessageReceiver *>> &record) {
                    if (auto it = std::find(record.second.begin(),
                                            record.second.end(),
                                            receiver); it
                        != record.second.end()) {
                      record.second.erase(it);
                    }
                  });
  }
  /**
   * Dispatch message to all listeners.
   * @tparam T type of message
   * @param command message
   */
  template<typename T>
  void dispatch(std::shared_ptr<T> command) {
    static_assert(std::is_base_of<Message, T>{});
    auto receivers = commands[typeid(*command.get())];
    std::for_each(receivers.begin(), receivers.end(), [&command](MessageReceiver *receiver) {
      receiver->receive(command);
    });
  }

 private:
  std::unordered_map<std::type_index, std::vector<MessageReceiver *>> commands;
};

#endif //PROJECT_COMMANDMANAGER_H
