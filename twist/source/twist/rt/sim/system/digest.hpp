#pragma once

#include <cstdint>

#include <twist/rt/sim/system/thread/struct.hpp>

#include <wheels/core/hash.hpp>

namespace twist::rt::sim {

namespace system {

using Digest = uint64_t;

class DigestCalculator {
 public:
  void Reset() {
    digest_ = 0;
  }

  void Load(uint64_t repr) {
    Feed(repr);
  }

  void Iter(Thread* thread) {
    Feed(thread->id);
  }

  size_t Digest() const noexcept {
    return digest_;
  }

 private:
  void Feed(size_t d) {
    wheels::HashCombine(digest_, d);
  }

 private:
  size_t digest_ = 0;
};

}  // namespace system

}  // namespace twist::rt::sim
