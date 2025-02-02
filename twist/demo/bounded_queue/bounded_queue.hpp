#pragma once

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/std/condition_variable.hpp>

#include <optional>
#include <queue>

// Bounded multi-producer multi-consumer blocking queue

template <typename T>
class BoundedQueue {
 public:
  explicit BoundedQueue(size_t capacity)
      : capacity_(capacity) {
  }

  void Put(T item) {
    std::unique_lock lock{mutex_};
    while (IsFull()) {
      cv_.wait(lock);
    }
    items_.push(std::move(item));
    cv_.notify_one();
  }

  std::optional<T> Take() {
    std::unique_lock lock{mutex_};
    while (IsEmpty() || closed_) {
      cv_.wait(lock);
    }
    if (!IsEmpty()) {
      T front = std::move(items_.front());
      items_.pop();
      cv_.notify_one();
      return front;
    } else {
      // Empty and closed
      return std::nullopt;
    }
  }

  void Close() {
    std::lock_guard guard{mutex_};
    closed_ = true;
    cv_.notify_all();
  }

 private:
  bool IsFull() const {
    return items_.size() == capacity_;
  }

  bool IsEmpty() const {
    return items_.empty();
  }

 private:
  const size_t capacity_;

  twist::ed::std::mutex mutex_;
  std::queue<T> items_;
  bool closed_{false};
  twist::ed::std::condition_variable cv_;
};
