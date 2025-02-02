#pragma once

namespace twist::rt::thr::test {

// TODO: Lock-free adversary

inline bool SetThreadAttrLockFree(bool /*ignore*/) {
  return false;  // No-op
}

//

struct LockFreeScope {
  LockFreeScope() {
    // No-op
  }

  ~LockFreeScope() {
    // No-op
  }
};

//

inline void Progress() {
  // No-op
}

}  // namespace twist::rt::thr::test
