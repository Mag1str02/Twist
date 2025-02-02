#pragma once

#include <twist/rt/sim/system/scheduler.hpp>

#include "params.hpp"

#include <twist/rt/sim/system/simulator.hpp>

#include <twist/wheels/random/wyrand.hpp>

#include <wheels/core/assert.hpp>
#include <wheels/intrusive/list.hpp>

#include <fmt/core.h>

#include <optional>

namespace twist::rt::sim {

namespace system::scheduler::coop {

class Scheduler final : public IScheduler {
 public:
  using Params = coop::Params;

 private:
  // Futex queue

  class WaitQueue : public IWaitQueue {
   public:
    bool IsEmpty() const override {
      return waiters_.IsEmpty();
    }

    void Push(Thread* waiter) override {
      return waiters_.PushBack(waiter);
    }

    Thread* Pop() override {
      return waiters_.PopFront();
    }

    Thread* PopAll() override {
      return Pop();
    }

    bool Remove(Thread* thread) override {
      if (waiters_.IsLinked(thread)) {
        waiters_.Unlink(thread);
        return true;
      } else {
        return false;
      }
    }

   private:
    wheels::IntrusiveList<Thread, SchedulerTag> waiters_;
  };

 public:
  Scheduler(Params params = Params())
      : params_(params),
        random_(params_.seed) {
    WHEELS_UNUSED(params);
  }

  bool NextSchedule() override {
    return false;
  }

  ~Scheduler() {
    WHEELS_VERIFY(run_queue_.IsEmpty(), "Non-empty run queue");
  }

  void Start(Simulator*) override {
    next_.reset();
    WHEELS_VERIFY(run_queue_.IsEmpty(), "Inconsistent state");
  }

  // System

  void Interrupt(Thread* active) override {
    run_queue_.PushFront(active);
  }

  void SwitchTo(Thread* active, ThreadId next) override {
    run_queue_.PushBack(active);
    next_ = next;
  }

  void Yield(Thread* active) override {
    run_queue_.PushBack(active);
  }

  void Wake(Thread* waiter) override {
    run_queue_.PushBack(waiter);
  }

  void Spawn(Thread* thread) override {
    run_queue_.PushBack(thread);
  }

  void Exit(Thread*) override {
    // No-op
  }

  // Run queue

  bool IsIdle() const override {
    return run_queue_.IsEmpty();
  }

  Thread* PickNext() override {
    if (next_) {
      ThreadId next_id = *next_;
      next_.reset();
      if (Thread* next = TryPick(next_id)) {
        return next;
      } else {
        WHEELS_PANIC("Cannot find target thread for SwitchTo");
      }
    } else {
      return PickFront();
    }
  }

  void Remove(Thread* thread) override {
    if (run_queue_.IsLinked(thread)) {
      run_queue_.Unlink(thread);
    }
  }

  // Hints

  void LockFree(Thread* /*thread*/, bool /*flag*/) override {
    // No-op
  }

  void Progress(Thread* /*thread*/) override {
    // No-op
  }

  void NewIter() override {
    // No-op
  }

  // Futex

  IWaitQueuePtr NewWaitQueue() override {
    return std::make_unique<WaitQueue>();
  }

  // Random

  uint64_t RandomNumber() override {
    return random_.Next();
  }

  size_t RandomChoice(size_t alts) override {
    return random_.Next() % alts;
  }

  // Spurious

  bool SpuriousWakeup() override {
    return false;
  }

  bool SpuriousTryFailure() override {
    return false;
  }

  // Description

  std::string Name() const override {
    return "CoopScheduler";
  }

 private:
  Thread* PickFront() {
    return run_queue_.PopFront();
  }

  Thread* TryPick(ThreadId id) {
    Thread* target = nullptr;

    for (Thread* t : run_queue_) {
      if (t->id == id) {
        target = t;
        break;
      }
    }

    if (target == nullptr) {
      return nullptr;
    } else {
      target->wheels::IntrusiveListNode<Thread, SchedulerTag>::Unlink();
      return target;
    }
  }

 private:
  const Params params_;
  twist::random::WyRand random_;

  wheels::IntrusiveList<Thread, SchedulerTag> run_queue_;
  std::optional<ThreadId> next_{};
};

}  // namespace system::scheduler::coop

}  // namespace twist::rt::sim
