#pragma once

#include <wheels/core/source_location.hpp>
#include <wheels/core/panic.hpp>

#include <atomic>
#include <cstddef>

namespace twist::rt::thr {

namespace test {

class Plate {
 public:
  Plate() = default;

  void Access(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
    if (access_.exchange(true, std::memory_order::relaxed)) {
      wheels::Panic("Mutual exclusion violated", call_site);
    }
    ++count_;
    access_.store(false, std::memory_order::relaxed);
  }

  size_t AccessCount() const {
    return count_;
  }

 private:
  std::atomic<bool> access_{false};
  size_t count_ = 0;
};

}  // namespace test

}  // namespace twist::rt::thr
