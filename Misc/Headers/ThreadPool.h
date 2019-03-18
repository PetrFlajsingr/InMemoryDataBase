//
// Created by Petr Flajsingr on 2019-02-24.
//

#ifndef PROJECT_DOWNLOADTHREADPOOL_H
#define PROJECT_DOWNLOADTHREADPOOL_H

#include <functional>
#include <vector>
#include <queue>
#include <condition_variable>
#include <thread>
#include <future>
#include <MessageReceiver.h>
#include <map>
/**
 * Synchronisation structure for tasks in the same group.
 */
struct TagSync {
  std::condition_variable cv;
  std::mutex mtx;
  std::size_t cnt = 0;
};
/**
 * Thread pool running executables either provided via enqueue method or
 * via a message.
 */
class ThreadPool final : public MessageReceiver {
 public:
  using Task = std::function<void()>;

 private:
  std::vector<std::thread> threads;
  std::queue<Task> tasks;

  std::condition_variable event;
  std::mutex mutex;
  bool stopping = false;

  std::map<int, TagSync> tagCounters;

 public:
  explicit ThreadPool(std::size_t numThreads);

  ~ThreadPool();

  ThreadPool(const ThreadPool &c) = delete;
  ThreadPool &operator=(const ThreadPool &c) = delete;

  template<class T>
  auto enqueue(T task) -> std::future<decltype(task())> {
    auto wrapper =
        std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task));

    {
      std::unique_lock<std::mutex> lock(mutex);
      tasks.emplace([=] {
        (*wrapper)();
      });
    }

    event.notify_one();
    return wrapper->get_future();
  }

  void wait(std::size_t tag);
 private:
  void start(std::size_t numThreads);

  void stop() noexcept;
  void receive(std::shared_ptr<Message> message) override;
};
#endif //PROJECT_DOWNLOADTHREADPOOL_H
