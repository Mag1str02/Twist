#pragma once

#include "../limits.hpp"
#include "../thread/count.hpp"

// https://github.com/llvm/llvm-project/blob/main/compiler-rt/lib/tsan/rtl/tsan_vector_clock.cpp


#include <wheels/core/assert.hpp>

#include <algorithm>
#include <cstdlib>
#include <cstdint>
#include <cstring>

namespace twist::rt::sim {

namespace system::sync {

using Epoch = uint32_t;

class VectorClock {
  static const size_t kClockSize = kMaxThreads;

 public:
  void Init() {
    std::memset(v_, 0, kClockSize * sizeof(Epoch));
  }

  Epoch Get(size_t tid) const {
    return v_[tid - 1];
  }

  Epoch Tick(size_t tid) {
    WHEELS_ASSERT(tid <= ThreadCount(), "Too many threads");
    return ++v_[tid - 1];
  }

  void Reset() {
    size_t n = ThreadCount();
    for (size_t i = 0; i < n; ++i) {
      v_[i] = 0;
    }
  }

  void Assign(const VectorClock& that) {
    size_t n = ThreadCount();
    for (size_t i = 0; i < n; ++i) {
      v_[i] = that.v_[i];
    }
  }

  void Join(const VectorClock& with) {
    size_t n = ThreadCount();
    for (size_t i = 0; i < n; ++i) {
      v_[i] = std::max(v_[i], with.v_[i]);
    }
  }

 private:
  Epoch v_[kClockSize];
};

}  // namespace system::sync

}  // namespace twist::rt::sim
