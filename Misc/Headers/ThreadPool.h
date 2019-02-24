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

class ThreadPool final {
 public:
  using Task = std::function<void()>;

 private:
  std::vector<std::thread> threads;
  std::queue<Task> tasks;

  std::condition_variable event;
  std::mutex mutex;
  bool stopping = false;

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

 private:
  void start(std::size_t numThreads);

  void stop() noexcept;
};
#endif //PROJECT_DOWNLOADTHREADPOOL_H
