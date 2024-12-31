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
  int num_threads;

  void run() 
  {
      while (true) 
      {
          TaskType task = task_queue.pop();
          task();
      }
  }

public:
  Threadpool(std::optional<int> thread_count = std::nullopt) 
  {
    num_threads = thread_count.has_value() ? thread_count.value() : std::thread::hardware_concurrency();
    std::cout << "Thread pool started with "<< num_threads << " number of threads...\n";
    
    for (int i = 0; i < num_threads; i++)
        threads.push_back(std::thread{&Threadpool::run, this});
  }

  void submit(TaskType&& task) 
  {
      task_queue.push(std::forward<TaskType>(task)); 
  }

  ~Threadpool() 
  {
      for (auto &th : threads)
          th.join();
  }
};



#endif