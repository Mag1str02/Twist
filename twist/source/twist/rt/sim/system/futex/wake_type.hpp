#pragma once

namespace twist::rt::sim {

namespace system {

enum class WakeType {
  SleepTimer,
  FutexWake,
  FutexTimer,
  Spurious,
};

}  // namespace system

}  // namespace twist::rt::sim
