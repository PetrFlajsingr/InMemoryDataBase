#include <utility>

//
// Created by Petr Flajsingr on 2019-02-25.
//

#ifndef PROJECT_COMMAND_H
#define PROJECT_COMMAND_H
#include <string>
#include <functional>

class Message {
 public:
  virtual ~Message() {}
};

template<typename T>
class DataMessage : public Message {
 protected:
  T data;
 public:
  T getData() const {
    return data;
  }
};

template<typename T>
class FncMesssage : public Message {
 protected:
  T fnc;
 public:
  T getFnc() const {
    return fnc;
  }
};

class ExecAsync : public FncMesssage<std::function<void()>> {
 public:
  explicit ExecAsync(std::function<void()> fnc) {
    FncMesssage::fnc = std::move(fnc);
  }
};

class ExecAsyncNotify : public FncMesssage<std::function<void()>> {
 public:
  ExecAsyncNotify(std::function<void()> fnc, std::function<void()> onDoneFnc) {
    FncMesssage::fnc = std::move(fnc);
    onDone = std::move(onDoneFnc);
  }

  std::function<void()> &getOnDone() {
    return onDone;
  }
 private:
  std::function<void()> onDone;
};

class StdinMsg : public DataMessage<std::string> {
 public:
  explicit StdinMsg(const std::string &data) {
    DataMessage::data = data;
  }
};

class UIEnd : public Message {};

class Download : public DataMessage<std::pair<std::string, std::string>> {
 public:
  explicit Download(std::pair<std::string, std::string> data) {
    DataMessage::data = std::move(data);
  }
  Download(const std::string &url, const std::string &dest) {
    DataMessage::data = std::make_pair(url, dest);
  }
};
class DownloadNoBlock : public Download {
 public:
  explicit DownloadNoBlock(const std::pair<std::string, std::string> &data)
      : Download(data) {}
  DownloadNoBlock(const std::string &url, const std::string &dest) : Download(
      url,
      dest) {}
};
enum class DownloadState {
  started, finished, failed
};
class DownloadProgress : public DataMessage<std::pair<DownloadState,
                                                      std::string>> {
 public:
  explicit DownloadProgress(const std::pair<DownloadState,
                                            std::string> &data) {
    DataMessage::data = data;
  }
  DownloadProgress(DownloadState state, const std::string &file) {
    DataMessage::data = std::make_pair(state, file);
  }
};

#endif //PROJECT_COMMAND_H
