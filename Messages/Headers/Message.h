#include <utility>

#include <utility>

//
// Created by Petr Flajsingr on 2019-02-25.
//

#ifndef PROJECT_COMMAND_H
#define PROJECT_COMMAND_H
#include <string>
#include <functional>

// Messages used to notify listeners or provide them with data/executable

/**
 * Base message class.
 */
class Message {
 public:
  virtual ~Message() = default;
};
/**
 * Message containing data.
 * @tparam T stored data type, can be anything
 */
template<typename T>
class DataMessage : public Message {
 protected:
  T data;
 public:
    [[nodiscard]] T getData() const {
    return data;
  }
};
/**
 * Message containing and executable.
 * @tparam T Function type
 */
template<typename T>
class FncMessage : public Message {
 protected:
  T fnc;
 public:
    explicit FncMessage(T fnc) : fnc(std::move(fnc)) {
    static_assert(std::is_invocable<T>::value, "T has to be invocable");
  }

    [[nodiscard]] T getFnc() const {
    return fnc;
  }
};
/**
 * Function to be run asynchronously.
 */
class ExecAsync : public FncMessage<std::function<void()>> {
 public:
  explicit ExecAsync(const std::function<void()> &fnc) : FncMessage(fnc) {}
};
/**
 * Function to be run asynchronously. Provide a tag to groupy tasks.
 */
class TaggedExecAsync : public FncMessage<std::function<void()>> {
 public:
  TaggedExecAsync(const std::function<void()> &fnc, int tag) : FncMessage(fnc), tag(tag) {}

  int getTag() {
    return tag;
  }
 private:
  int tag;
};
/**
 * Execute asynchronously and call onDone when the function is finished.
 */
class ExecAsyncNotify : public FncMessage<std::function<void()>> {
 public:
  ExecAsyncNotify(std::function<void()> fnc, std::function<void()> onDoneFnc) : FncMessage(std::move(fnc)) {
    onDone = std::move(onDoneFnc);
  }

  std::function<void()> &getOnDone() {
    return onDone;
  }
 private:
  std::function<void()> onDone;
};
/**
 * Same as above but with group tag.
 */
class TaggedExecAsyncNotify : public ExecAsyncNotify {
 public:
  TaggedExecAsyncNotify(const std::function<void()> &fnc, const std::function<void()> &onDoneFnc, int tag)
      : ExecAsyncNotify(fnc, onDoneFnc), tag(tag) {}

  int getTag() {
    return tag;
  }
 private:
  int tag;
};
/**
 * Stdin input notification.
 */
class StdinMsg : public DataMessage<std::string> {
 public:
  explicit StdinMsg(const std::string &data) {
    DataMessage::data = data;
  }
};
/**
 * End UI notification.
 */
class UIEnd : public Message {};
/**
 * Download a file with blocking.
 */
class Download : public DataMessage<std::pair<std::string, std::string>> {
 public:
  explicit Download(std::pair<std::string, std::string> data) {
    DataMessage::data = std::move(data);
  }
  Download(const std::string &url, const std::string &dest) {
    DataMessage::data = std::make_pair(url, dest);
  }
};
/**
 * Print message to stdout.
 */
class Print : public DataMessage<std::string> {
 public:
  explicit Print(const std::string &data) {
    DataMessage::data = data;
  }
};
/**
 * Download file without blocking the caller.
 */
class DownloadNoBlock : public Download {
 public:
  explicit DownloadNoBlock(const std::pair<std::string, std::string> &data) : Download(data) {}
  DownloadNoBlock(const std::string &url, const std::string &dest) : Download(url, dest) {}
};
enum class DownloadState {
  started, finished, failed
};
/**
 * Provide download progress information.
 */
class DownloadProgress : public DataMessage<std::pair<DownloadState, std::string>> {
 public:
  explicit DownloadProgress(const std::pair<DownloadState, std::string> &data) {
    DataMessage::data = data;
  }
  DownloadProgress(DownloadState state, const std::string &file) {
    DataMessage::data = std::make_pair(state, file);
  }
};

#endif //PROJECT_COMMAND_H
