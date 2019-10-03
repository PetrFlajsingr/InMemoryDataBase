//
// Created by Petr Flajsingr on 2019-02-23.
//

#include <ConsoleIO.h>

ConsoleIO::ConsoleIO(const std::shared_ptr<MessageManager> &commandManager) : MessageSender(commandManager) {
  commandManager->registerMsg<UIEnd>(this);
  commandManager->registerMsg<Print>(this);
}

void ConsoleIO::write(std::string_view str) {
  std::unique_lock lck{mutex};
  std::cout << ConsoleIO::form << str;
}
void ConsoleIO::writeLn(std::string_view str) {
  std::unique_lock lck{mutex};
  std::cout << ConsoleIO::form << str << std::endl;
  if (*mode == Mode::arrow) {
    std::cout << ConsoleIO::form;
  }
}
void ConsoleIO::writeErr(std::string_view str) {
  std::unique_lock lck{mutex};
  std::cerr << ConsoleIO::form << str;
}
void ConsoleIO::writeLnErr(std::string_view str) {
  std::unique_lock lck{mutex};
  std::cerr << ConsoleIO::form << str << std::endl;
}
std::string ConsoleIO::readLn() {
  std::string str;
  std::getline(std::cin, str);
  return str;
}
void ConsoleIO::setMode(ConsoleIO::Mode mode) {
  switch (mode) {
    case Mode::simple:ConsoleIO::form = "";
      break;
    case Mode::arrow:ConsoleIO::form = ">> ";
      write("");
      break;
  }
}

void ConsoleIO::listenInput() {
  while (listen) {
    dispatch(new StdinMsg(readLn()));
  }
}
void ConsoleIO::receive(std::shared_ptr<Message> message) {
  if (auto msg = std::dynamic_pointer_cast<UIEnd>(message)) {
    listen = false;
    writeLn("Press enter to exit...");
  }
}
ConsoleIO::~ConsoleIO() {
  if (auto tmp = commandManager.lock()) {
    tmp->unregister(this);
  }
}
