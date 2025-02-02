#pragma once

#include "../scheduler/lock_free.hpp"

namespace twist::rt::sim {

namespace user::test {

inline bool SetThreadAttrLockFree(bool on) {
  return scheduler::SetThreadAttrLockFree(on);
}

//

struct LockFreeScope {
  LockFreeScope() {
    prev_status_ = SetThreadAttrLockFree(true);
  }

  ~LockFreeScope() {
    // Restore
    SetThreadAttrLockFree(prev_status_);
  }

 private:
  bool prev_status_;
};

//

inline void Progress() {
  scheduler::ReportProgress();
}

}  // namespace user::test

}  // namespace twist::rt::sim
