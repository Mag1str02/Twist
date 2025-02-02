#pragma once

namespace twist::rt::sim {

namespace system {

enum class ThreadState {
  Starting,  // Initial state
  Runnable,  // In Scheduler run queue
  Running,
  Sleeping,
  LfSuspended,
  Parked,
  Terminated,
  Deadlocked,
};

}  // namespace system

}  // namespace twist::rt::sim
