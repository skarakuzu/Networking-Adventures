#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

template <typename T> class Queue {

  std::mutex mtx;
  std::queue<T> queue;
  std::condition_variable cond_var;

public:
  Queue() = default;

  void push(T&& value) {
    std::lock_guard<std::mutex> lck(mtx);
    queue.push(std::forward<T>(value));
    cond_var.notify_all();
  }

  T pop() {
    std::unique_lock<std::mutex> lck(mtx);
    cond_var.wait(lck, [this] { return !queue.empty(); });

    T value{std::forward<T>(queue.front())};
    queue.pop();
    return value;
  }
};

#endif