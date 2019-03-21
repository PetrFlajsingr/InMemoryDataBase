//
// Created by Petr Flajsingr on 2019-02-23.
//

#ifndef PROJECT_CONSOLEIO_H
#define PROJECT_CONSOLEIO_H

#include <iostream>
#include <string>
#include <MessageSender.h>
#include <Property.h>
#include <memory>

class ConsoleIO : public MessageSender, public MessageReceiver {
 public:
  explicit ConsoleIO(const std::shared_ptr<MessageManager> &commandManager);
  virtual ~ConsoleIO();

  void listenInput();

  void write(std::string_view str);
  void writeLn(std::string_view str);
  void writeErr(std::string_view str);
  void writeLnErr(std::string_view str);
  std::string readLn();

  enum class Mode {
    simple, arrow
  };

  Property<Mode, ConsoleIO, RW> mode{
      [this]() -> Mode & { return mode.value; },
      [this](const Mode &val) -> Mode & {
        mode.value = val;
        setMode(val);
        return mode.value;
      }
  };

  void setMode(Mode mode);

 private:
  std::mutex mutex;
  void receive(std::shared_ptr<Message> message) override;
  std::string form;
  bool listen = true;
};

#endif //PROJECT_CONSOLEIO_H
