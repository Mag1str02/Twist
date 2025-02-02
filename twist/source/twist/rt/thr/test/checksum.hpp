#pragma once

#include <atomic>
#include <functional>  // std::hash

namespace twist::rt::thr {

namespace test {

// For producer/consumer tests

template <typename T>
class CheckSum {
 public:
  void Produce(const T& item) {
    Feed(item);
  }

  void Consume(const T& item) {
    Feed(item);
  }

  bool Validate() {
    return value_.load() == 0;
  }

  size_t Value() const {
    return value_.load();
  }

 private:
  static size_t Hash(const T& item) {
    return std::hash<T>{}(item);
  }

  void Feed(const T& item) {
    value_.fetch_xor(Hash(item), std::memory_order::relaxed);
  }

 private:
  std::atomic_size_t value_ = 0;
};

}  // namespace test

}  // namespace twist::rt::thr
