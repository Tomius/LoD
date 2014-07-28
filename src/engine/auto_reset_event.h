// Copyright (c) 2014, Tamas Csala

#ifndef ENGINE_AUTO_RESET_EVENT_H
#define ENGINE_AUTO_RESET_EVENT_H

#include <thread>
#include <mutex>
#include <condition_variable>

namespace engine {

class AutoResetEvent {
 public:
  explicit AutoResetEvent(bool initial = false) : flag_(initial) {}

  void set() {
    std::lock_guard<std::mutex> lock(mutex_);
    flag_ = true;
    signal_.notify_one();
  }

  void reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    flag_ = false;
  }

  bool waitOne() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (!flag_)  // prevents false wakeups from doing harm
      signal_.wait(lock);
    flag_ = false;
    return true;
  }

 private:
  bool flag_;
  std::mutex mutex_;
  std::condition_variable signal_;

  AutoResetEvent(const AutoResetEvent&) = delete;
  AutoResetEvent& operator=(const AutoResetEvent&) = delete;
};

}  // namespace engine

#endif
