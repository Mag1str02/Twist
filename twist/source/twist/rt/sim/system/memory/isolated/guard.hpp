#pragma once

#include <cstddef>

namespace twist::rt::sim {

namespace system {

namespace memory::isolated {

class UserAllocGuard {
 public:
  UserAllocGuard()
      : before_(AllowUserAlloc(false)) {
  }

  ~UserAllocGuard() {
    AllowUserAlloc(before_);
  }

 private:
  static bool AllowUserAlloc(bool flag);

 private:
  bool before_;
};

}  // namespace memory::isolated

}  // namespace system

}  // namespace twist::rt::sim
