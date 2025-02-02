#pragma once

// uint32_t
#include <cstdint>
// size_t
#include <cstdlib>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

namespace heap {

using SizeClass = uint8_t;

class SizeClassifier {
 public:
  static SizeClass GetSizeClass(size_t user_size) {
    SizeClass klass = GetSizeClassSlow(user_size);
    return AtLeast16Bytes(klass);
  }

  static size_t BlockSize(SizeClass klass) {
    return (size_t)1 << klass;
  }

 private:
  static SizeClass GetSizeClassSlow(size_t user_size) {
    // Most allocations are small
    for (SizeClass k = 4; k < 32; ++k) {
      if (BlockSize(k) >= user_size) {
        return k;
      }
    }
    std::abort();  // Too big to fail
  }

  static SizeClass AtLeast16Bytes(SizeClass klass) {
    if (klass < 4) {
      return 4;
    } else {
      return klass;
    }
  }
};

}  // namespace heap

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
