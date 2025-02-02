#pragma once

namespace twist::rt::sim {

namespace system {

enum class FutexType {
  Futex,
  Atomic,
  Thread,
  MutexLock,
  MutexTryLock,
  CondVar,
  WaitGroup,
};

}  // namespace system

}  // namespace twist::rt::sim
