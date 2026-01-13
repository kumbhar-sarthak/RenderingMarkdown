#ifndef PARALLEL_H_
#define PARALLEL_H_

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>             
#include <vector>             

class ThreadPool {
 public:
  explicit ThreadPool(size_t num_threads = std::thread::hardware_concurrency());
  
  ~ThreadPool();

  void enqueue(std::function<void()> task);

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

 private:
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;

  std::mutex queue_mutex_;
  std::condition_variable condition_;
  bool stop_;
};

#endif  // PARALLEL_H_