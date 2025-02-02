#pragma once

#include "../scheduler/wait_queue.hpp"

#include <wheels/core/assert.hpp>

#include <utility>

namespace twist::rt::sim {

namespace system {

class SingletonWaitQueue {
 public:
  SingletonWaitQueue() = default;

  SingletonWaitQueue(SingletonWaitQueue&& that)
      : waiter_(std::exchange(that.waiter_, nullptr)) {
  }

  bool IsEmpty() const {
    return waiter_ == nullptr;
  }

  bool TryPush(Thread* new_waiter) {
    if (waiter_ == nullptr) {
      waiter_ = new_waiter;
      return true;
    } else {
      return false;
    }
  }

  Thread* TryPop() {
    return std::exchange(waiter_, nullptr);
  }

  Thread* Pop() {
    WHEELS_ASSERT(!IsEmpty(), "SingletonWaitQueue is empty");
    return TryPop();
  }

  bool Remove(Thread* waiter) {
    return std::exchange(waiter_, nullptr) == waiter;
  }

 private:
  Thread* waiter_ = nullptr;
};

class WaitQueue {
  friend class Simulator;

 public:
  WaitQueue() noexcept {
  }

  void Init() {
    singleton_.TryPop();
    waiters_.reset();
  }

  WaitQueue(WaitQueue&& that) = default;

  ~WaitQueue() {
    WHEELS_VERIFY(IsEmpty(), "Destroying non-empty wait queue");
  }

  void Push(Thread* new_waiter) {
    if (waiters_) {
      // Scheduler queue
      waiters_->Push(new_waiter);
    } else if (singleton_.TryPush(new_waiter)) {
      // Singleton queue
    } else {
      // Switch from singleton queue to scheduler queue
      AllocateSchedulerQueue();
      waiters_->Push(singleton_.Pop());
      waiters_->Push(new_waiter);
    }
  }

  Thread* Pop() {
    if (waiters_) {
      return waiters_->Pop();
    } else {
      return singleton_.TryPop();
    }
  }

  Thread* PopAll() {
    if (waiters_) {
      return waiters_->PopAll();
    } else {
      return singleton_.TryPop();
    }
  }

  bool IsEmpty() const {
    return waiters_
               ? waiters_->IsEmpty()
               : singleton_.IsEmpty();
  }

  void Remove(Thread* waiter) {
    bool removed = waiters_
                       ? waiters_->Remove(waiter)
                       : singleton_.Remove(waiter);

    WHEELS_VERIFY(removed, "Waiter not found in WaitQueue");
  }

 private:
  void AllocateSchedulerQueue();

 private:
  IWaitQueuePtr waiters_;
  SingletonWaitQueue singleton_;
};

}  // namespace system

}  // namespace twist::rt::sim
