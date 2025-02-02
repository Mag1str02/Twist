#pragma once

#include <cstddef>
#include <functional>  // std::hash

namespace twist::rt::sim {

namespace user::test {

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
    return Value() == 0;
  }

  size_t Value() const {
    return value_;
  }

 private:
  static size_t Hash(const T& item) {
    return std::hash<T>{}(item);
  }

  void Feed(const T& item) {
    value_ ^= Hash(item);
  }

 private:
  size_t value_ = 0;
};

}  // namespace user::test

}  // namespace twist::rt::sim
