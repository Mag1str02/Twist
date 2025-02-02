#pragma once

#include "../fwd.hpp"
#include "../limits.hpp"
#include "../thread/id.hpp"

#include "run_queue.hpp"
#include "wait_queue.hpp"

#include <cstdint>
#include <cstdlib>

namespace twist::rt::sim {

namespace system {

struct IScheduler : IRunQueue {
  virtual ~IScheduler() = default;

  virtual bool NextSchedule() = 0;

  virtual void Start(Simulator*) = 0;

  virtual void Interrupt(Thread*) = 0;
  virtual void SwitchTo(Thread*, ThreadId) = 0;
  virtual void Yield(Thread*) = 0;
  virtual void Wake(Thread*) = 0;
  virtual void Spawn(Thread*) = 0;
  virtual void Exit(Thread*) = 0;

  // Hints

  virtual void LockFree(Thread*, bool flag) = 0;
  virtual void Progress(Thread*) = 0;
  virtual void NewIter() = 0;

  // Futex

  virtual IWaitQueuePtr NewWaitQueue() = 0;

  // Random

  virtual uint64_t RandomNumber() = 0;
  virtual size_t RandomChoice(size_t alts) = 0;

  // Spurious

  virtual bool SpuriousWakeup() = 0;

  // mutex::try_lock, atomic<T>::compare_exchange_weak
  virtual bool SpuriousTryFailure() = 0;

  // SpinWait
  virtual bool IsModelChecker() const {
    return false;
  }

  // Description

  virtual std::string Name() const = 0;
};

}  // namespace system

}  // namespace twist::rt::sim
