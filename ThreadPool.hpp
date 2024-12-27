#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

#include "Queue.hpp"


template<typename TaskType>
class Threadpool {
  std::vector<std::thread> threads;
  Queue<TaskType> task_queue;
  void run() {
    while (true) {
      TaskType task = task_queue.pop();
      task();
    }
  }

public:
  Threadpool() {
    int thread_count = std::thread::hardware_concurrency();
    std::cout << "number of hardware threads " << thread_count << std::endl;
    for (int i = 0; i < thread_count; i++)
      threads.push_back(std::thread{&Threadpool::run, this});
  }

  void submit(TaskType&& task) {
   task_queue.push(std::forward<TaskType>(task)); 
   }

  ~Threadpool() {
    for (auto &th : threads)
      th.join();
  }
};



#endif