#pragma once

#include <twist/rt/sim/system/scheduler.hpp>

#include "params.hpp"
#include "schedule.hpp"

#include <twist/rt/sim/system/simulator.hpp>

#include <wheels/core/assert.hpp>

#include <map>

namespace twist::rt::sim {

namespace system::scheduler::replay {

class Scheduler final : public IScheduler {
 public:
  using Params = replay::Params;
  using Schedule = replay::Schedule;

 private:
  // Futex queue

  class WaitQueue : public IWaitQueue {
    friend class Scheduler;

   public:
    WaitQueue(Scheduler* scheduler)
        : scheduler_(scheduler) {
    }

    bool IsEmpty() const override {
      return set_.empty();
    }

    void Push(Thread* waiter) override {
      set_.try_emplace(waiter->id, waiter);
    }

    Thread* Pop() override {
      return scheduler_->WakeOne(this);
    }

    Thread* PopAll() override {
      return scheduler_->WakeAll(this);
    }

    bool Remove(Thread* thread) override {
      return set_.erase(thread->id) > 0;
    }

   private:
    Thread* PopById(ThreadId id) {
      auto it = set_.find(id);
      WHEELS_VERIFY(it != set_.end(), "Fiber not found in wait queue");
      Thread* f = it->second;
      set_.erase(it);
      return f;
    }

   private:
    std::map<ThreadId, Thread*> set_;
    Scheduler* scheduler_;
  };

 public:
  Scheduler(Schedule schedule)
      : schedule_(schedule) {
  }

  bool NextSchedule() override {
    return false;
  }

  ~Scheduler() {
    WHEELS_VERIFY(run_set_.empty(), "Non-empty run queue");
  }

  void Start(Simulator*) override {
    // No-op
  }

  // System

  void Interrupt(Thread* active) override {
    run_set_.try_emplace(active->id, active);
  }

  void SwitchTo(Thread* active, ThreadId /*next*/) override {
    run_set_.try_emplace(active->id, active);
  }

  void Yield(Thread* active) override {
    run_set_.try_emplace(active->id, active);
  }

  void Wake(Thread* waiter) override {
    run_set_.try_emplace(waiter->id, waiter);
  }

  void Spawn(Thread* thread) override {
    run_set_.try_emplace(thread->id, thread);
  }

  void Exit(Thread*) override {
    // No-op
  }

  // Run queue

  bool IsIdle() const override {
    return run_set_.empty();
  }

  Thread* PickNext() override {
    auto* next = Next<decision::PickNext>();

    {
      auto it = run_set_.find(next->id);
      WHEELS_VERIFY(it != run_set_.end(), "Next thread not found in run queue");
      Thread* f = it->second;
      run_set_.erase(next->id);
      return f;
    }
  }

  void Remove(Thread* thread) override {
    run_set_.erase(thread->id);
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
    return std::make_unique<WaitQueue>(this);
  }

  // Random

  uint64_t RandomNumber() override {
    return Next<decision::RandomNumber>()->value;
  }

  size_t RandomChoice(size_t alts) override {
    auto* choice = Next<decision::RandomChoice>();
    WHEELS_VERIFY(choice->index < alts, "Alts mismatch");
    return choice->index;
  }

  // Spurious

  bool SpuriousWakeup() override {
    return Next<decision::SpuriousWakeup>()->wake;
  }

  bool SpuriousTryFailure() override {
    return Next<decision::SpuriousTryFailure>()->fail;
  }

  // Description

  std::string Name() const override {
    return "ReplayScheduler";
  }

 private:
  Thread* WakeOne(WaitQueue* wq) {
    auto* wake = Next<decision::WakeOne>();

    if (wake->id) {
      return wq->PopById(*(wake->id));
    } else {
      return nullptr;
    }
  }

  Thread* WakeAll(WaitQueue* wq) {
    auto* wake = Next<decision::WakeAll>();

    if (wake->id) {
      return wq->PopById(*(wake->id));
    } else {
      return nullptr;
    }
  }

 private:
  template <typename T>
  T* Next() {
    WHEELS_VERIFY(next_ < schedule_.size(), "Unexpected schedule length");
    T* next = std::get_if<T>(&schedule_[next_++]);
    WHEELS_VERIFY(next != nullptr, "Decision type mismatch");
    return next;
  }

 private:
  Schedule schedule_;
  size_t next_ = 0;
  std::map<ThreadId, Thread*> run_set_;
};

}  // namespace system::scheduler::replay

}  // namespace twist::rt::sim
