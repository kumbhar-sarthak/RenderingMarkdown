#include "parallel.h"
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

ThreadPool::ThreadPool(size_t num_threads) : stop_(false) {
  for(int i = 0; i < num_threads; i++){
    workers_.emplace_back([this]{
      while (true)
      {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(this->queue_mutex_);
          this->condition_.wait(lock, [this] {
            return !this->tasks_.empty() || this->stop_;
          });
          if (this->stop_ && this->tasks_.empty()) {
            return;
          }

          task = std::move(this->tasks_.front());
          this->tasks_.pop();
        }

        task();
      }
      
    });
  }
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(this->queue_mutex_);
    this->stop_ = true;
  }

  this->condition_.notify_all();

  for(auto& worker : this->workers_){
    worker.join();
  }
}

void ThreadPool::enqueue(std::function<void()> task) {
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    tasks_.emplace(std::move(task));
  }
  condition_.notify_one();
}
