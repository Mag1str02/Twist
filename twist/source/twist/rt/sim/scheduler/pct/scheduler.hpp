#pragma once

#include <twist/rt/sim/system/scheduler.hpp>

#include "params.hpp"

#include <twist/rt/sim/system/simulator.hpp>

#include <twist/wheels/random/wyrand.hpp>

#include <wheels/core/assert.hpp>
#include <wheels/core/hash.hpp>
#include <wheels/intrusive/list.hpp>

#include <fmt/core.h>

#include <vector>
#include <queue>
#include <set>

namespace twist::rt::sim {

namespace system::scheduler::pct {

class Scheduler final : public IScheduler {
  using PriorityValue = int32_t;

 public:
  using Params = pct::Params;

 private:
  // Run queue

  class RunQueue {
    struct Entry {
      Thread* thread;

      PriorityValue Priority() const {
        return Scheduler::GetPriority(thread);
      }

      bool operator<(const Entry& that) const {
        return this->Priority() < that.Priority();
      }
    };

   public:
    bool IsEmpty() const {
      return queue_.empty();
    }

    void Push(Thread* thread) {
      queue_.push(Entry{thread});
    }

    Thread* PopMaxPriority() {
      Entry top = queue_.top();
      queue_.pop();
      return top.thread;
    }

    PriorityValue MaxPriority() const {
      WHEELS_ASSERT(!IsEmpty(), "Empty run queue");
      Entry top = queue_.top();
      return top.Priority();
    }

    void Remove(Thread*) {
      // TODO
      // Do nothing
    }

   private:
    std::priority_queue<Entry> queue_;
  };

  // Futex queue

  class WaitQueue : public IWaitQueue {
   public:
    bool IsEmpty() const override {
      return queue_.IsEmpty();
    }

    void Push(Thread* waiter) override {
      return queue_.PushBack(waiter);
    }

    Thread* Pop() override {
      return queue_.PopFront();
    }

    Thread* PopAll() override {
      return Pop();
    }

    bool Remove(Thread* thread) override {
      if (queue_.IsLinked(thread)) {
        queue_.Unlink(thread);
        return true;
      } else {
        return false;
      }
    }

   private:
    wheels::IntrusiveList<Thread, SchedulerTag> queue_;
  };

 public:
  Scheduler(Params params = Params())
      : params_(params) {
  }

  bool NextSchedule() override {
    if (params_.max_runs && runs_ + 1 >= *params_.max_runs) {
      return false;
    }

    wheels::HashCombine(seed_, ++runs_);
    return true;
  }

  // IScheduler

  void Start(Simulator*) override {
    random_.Seed(seed_);
    Reset(estimates_.threads, estimates_.steps, params_.depth);

    active_ = nullptr;
    WHEELS_VERIFY(run_queue_.IsEmpty(), "Inconsistent state");
  }

  // System

  void Interrupt(Thread* active) override {
    active_ = active;
  }

  void SwitchTo(Thread* /*active*/, ThreadId /*next*/) override {
    WHEELS_PANIC("SwitchTo not supported by PctScheduler");
  }

  void Yield(Thread* thread) override {
    SetPriority(thread, NextPriority());
    run_queue_.Push(thread);
  }

  void Wake(Thread* waiter) override {
    run_queue_.Push(waiter);
  }

  void Spawn(Thread* thread) override {
    if (thread->id < init_prior_.size()) {
      SetPriority(thread, init_prior_[thread->id]);
    } else {
      estimates_.threads = std::max(estimates_.threads, (size_t)thread->id);
      SetPriority(thread, NextPriority());
    }
    run_queue_.Push(thread);
  }

  void Exit(Thread*) override {
    // No-op
  }

  // Run queue

  bool IsIdle() const override {
    return run_queue_.IsEmpty() && (active_ == nullptr);
  }

  Thread* PickNext() override {
    return PickNext(std::exchange(active_, nullptr));
  }

  void Remove(Thread* thread) override {
    run_queue_.Remove(thread);
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
    return "PctScheduler";
  }

 private:
  Thread* PickNext(Thread* active) {
    ++iter_;

    estimates_.steps = std::max(estimates_.steps, iter_);

    if (active != nullptr) {
      if (!Preempt(active)) {
        return active;
      } else {
        run_queue_.Push(active);
      }
    }

    return run_queue_.PopMaxPriority();
  }

  bool Preempt(Thread* active) {
    if (!active->attrs.preemptive) {
      return false;  // Continue
    }

    size_t iter = iter_;

    while (iter > change_points_[next_change_point_]) {
      ++next_change_point_;  // Skip
    }

    if (iter == change_points_[next_change_point_]) {
      // Change priority
      SetPriority(active, NextPriority());
      return true;
    }

    if (run_queue_.IsEmpty()) {
      return false;  // Alone
    }

    if (GetPriority(active) < run_queue_.MaxPriority()) {
      return true;
    }

    return false;
  }

  static PriorityValue GetPriority(Thread* f) {
    return (PriorityValue)f->sched.f1;
  }

  static void SetPriority(Thread* f, PriorityValue p) {
    f->sched.f1 = (uintptr_t)p;
  }

  void Reset(size_t threads, size_t length, size_t depth) {
    GenerateInitPriorities(threads);
    GenerateChangePoints(length, depth);
    next_change_point_ = 0;
    iter_ = 0;
  }

  void GenerateInitPriorities(size_t n) {
    init_prior_.clear();

    // 1-indexed
    init_prior_.push_back(0);

    for (size_t i = 1; i <= n; ++i) {
      init_prior_.push_back((int)i);
    }

    // https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
    for (size_t i = n; i > 1; --i) {
      size_t j = 1 + random_.Next() % i;
      std::swap(init_prior_[i], init_prior_[j]);
    }

    next_prior_ = 0;
  }

  void GenerateChangePoints(size_t length, size_t depth) {
    std::set<size_t> change_points;
    while (change_points.size() < depth - 1) {
      size_t iter = 1 + random_.Next() % length;
      change_points.insert(iter);
    }

    change_points_.clear();
    for (size_t step : change_points) {
      change_points_.push_back(step);
    }

    // Guard value to prevent off-by-one errors
    change_points_.push_back(std::numeric_limits<size_t>::max());
  }

  PriorityValue NextPriority() {
    return next_prior_--;
  }

 private:
  const Params params_;

  size_t seed_;
  size_t runs_ = 0;

  Estimates estimates_ = {.threads = 1, .steps = 7};
  
  std::vector<PriorityValue> init_prior_;
  PriorityValue next_prior_;
  std::vector<size_t> change_points_;
  size_t next_change_point_;

  twist::random::WyRand random_{42};
  RunQueue run_queue_;
  Thread* active_ = nullptr;
  size_t iter_;
};

}  // namespace system::scheduler::pct

}  // namespace twist::rt::sim
