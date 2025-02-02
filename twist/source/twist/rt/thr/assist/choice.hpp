#pragma once

#include <random>

#include <wheels/core/assert.hpp>

#include <cstdlib>

namespace twist::rt::thr {

namespace assist {

class Choice {
 public:
  explicit Choice(std::random_device& rd)
      : twister_(rd()) {
  }

  // Non-copyable
  Choice(const Choice&) = delete;
  Choice& operator=(const Choice&) = delete;

  // Non-movable
  Choice(Choice&&) = delete;
  Choice& operator=(Choice&&) = delete;

  // [0, alts)
  size_t operator()(size_t alts) {
    return Impl(alts);
  }

  // [lo, hi)
  size_t operator()(size_t lo, size_t hi) {
    WHEELS_ASSERT(lo < hi, "Invalid index range");
    return lo + Impl(hi - lo);
  }

 private:
  size_t Impl(size_t alts) {
    auto distr = std::uniform_int_distribution<uint32_t>(0, alts - 1);
    return distr(twister_);
  }

 private:
  std::mt19937 twister_;
};

}  // namespace assist

}  // namespace twist::rt::thr
