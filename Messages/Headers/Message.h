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
class FncMessage : public Message {
 protected:
  T fnc;
 public:
  explicit FncMessage(T fnc) : fnc(fnc) {}
  T getFnc() const {
    return fnc;
  }
};

class ExecAsync : public FncMessage<std::function<void()>> {
 public:
  ExecAsync(const std::function<void()> &fnc) : FncMessage(fnc) {}
};

class TaggedExecAsync : public FncMessage<std::function<void()>> {
 public:
  TaggedExecAsync(const std::function<void()> &fnc, int tag)
      : FncMessage(fnc), tag(tag) {}

  int getTag() {
    return tag;
  }
 private:
  int tag;
};

class ExecAsyncNotify : public FncMessage<std::function<void()>> {
 public:
  ExecAsyncNotify(std::function<void()> fnc,
                  std::function<void()> onDoneFnc) : FncMessage(fnc) {
    onDone = std::move(onDoneFnc);
  }

  std::function<void()> &getOnDone() {
    return onDone;
  }
 private:
  std::function<void()> onDone;
};

class TaggedExecAsyncNotify : public ExecAsyncNotify {
 public:
  TaggedExecAsyncNotify(const std::function<void()> &fnc,
                        const std::function<void()> &onDoneFnc,
                        int tag) : ExecAsyncNotify(fnc, onDoneFnc), tag(tag) {}

  int getTag() {
    return tag;
  }
 private:
  int tag;
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
