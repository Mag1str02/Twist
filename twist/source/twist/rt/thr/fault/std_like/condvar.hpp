#pragma once

#include "mutex.hpp"

#include <twist/rt/thr/fault/random/helpers.hpp>

#include <twist/rt/thr/wait/futex/wait.hpp>

#include <wheels/intrusive/list.hpp>

#include <optional>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace twist::rt::thr {
namespace fault {

namespace detail {

class OneShotEvent {
 public:
  void Wait() {
    rt::thr::futex::Wait(fired_, 0);
  }

  // true - woken, false - timeout
  bool WaitTimed(std::chrono::milliseconds timeout) {
    auto [woken, _] = rt::thr::futex::WaitTimed(fired_, 0, timeout);
    return woken;
  }

  void Fire() {
    auto wake_key = rt::thr::futex::PrepareWake(fired_);
    fired_.store(1);
    rt::thr::futex::WakeOne(wake_key);
  }

 private:
  std::atomic<uint32_t> fired_{0};
};

}  // namespace detail

class FaultyCondVar {
  using Clock = std::chrono::system_clock;
  using Timeout = std::chrono::milliseconds;
  using MaybeTimeout = std::optional<std::chrono::milliseconds>;

  using Lock = std::unique_lock<FaultyMutex>;

 private:
  struct Waiter : public wheels::IntrusiveListNode<Waiter>,
                  public detail::OneShotEvent {
    void Wake() {
      Fire();
    }
  };

  using WaitQueue = wheels::IntrusiveList<Waiter>;

 public:
  FaultyCondVar() {
    AccessAdversary();
  }

  void wait(Lock& lock) {  // NOLINT
    Wait(lock);
  }

  template <class Predicate>
  void wait(Lock& lock, Predicate predicate) {  // NOLINT
    while (!predicate()) {
      wait(lock);
    }
  }

  std::cv_status wait_for(Lock& lock, std::chrono::milliseconds timeout) {
    return WaitTimed(lock, timeout);
  }

  std::cv_status wait_until(Lock& lock, std::chrono::system_clock::time_point expiration_time) {
    return WaitTimed(lock, expiration_time);
  }

  void notify_one() {  // NOLINT
    NotifyOne();
  }

  void notify_all() {  // NOLINT
    NotifyAll();
  }

 private:
  std::cv_status FaultyWaitTimed(Lock &lock, MaybeTimeout timeout) {
    ++wait_call_count_;

    // Spurious wakeup
    if (wait_call_count_ % 13 == 0) {
      return std::cv_status::no_timeout;
    }

    auto woken = WaitImpl(lock, timeout);

    if (wait_call_count_ % 5 == 0) {
      // Give lock to contender thread, try to provoke intercepted wakeup
      lock.unlock();
      ::std::this_thread::yield();
      lock.lock();
    }

    return woken ? std::cv_status::no_timeout
                 : std::cv_status::timeout;
  }

  void Wait(Lock& lock) {
    FaultyWaitTimed(lock, std::nullopt);
  }

  std::cv_status WaitTimed(Lock& lock, Timeout timeout) {
    return FaultyWaitTimed(lock, {timeout});
  }

  static Timeout ToTimeout(Clock::time_point expiration_time) {
    auto now = Clock::now();
    if (expiration_time > now) {
      return std::chrono::duration_cast<Timeout>(expiration_time - now);
    } else {
      return Timeout{0};
    }
  }

  std::cv_status WaitTimed(Lock& lock, Clock::time_point expiration_time) {
    return FaultyWaitTimed(lock, ToTimeout(expiration_time));
  }

  // Notify random thread from waiting queue
  void NotifyOne() {
    std::unique_lock waiters_lock(waiters_mutex_);

    if (waiters_.IsEmpty()) {
      return;
    }

    auto *waiter = UnlinkRandomItem(waiters_);
    waiter->Wake();
  }

  // Notify all waiting threads in random order
  void NotifyAll() {
    std::unique_lock waiters_lock(waiters_mutex_);

    auto wake_order = ShuffleToVector(waiters_);
    for (Waiter *waiter : wake_order) {
      waiter->Wake();
    }
  }

 private:
  // true - woken, false - timeout
  bool WaitImpl(Lock& lock, MaybeTimeout timeout) {
    std::unique_lock waiters_lock(waiters_mutex_);

    // Release external mutex
    lock.unlock();

    // allocate on stack!
    Waiter waiter;

    waiters_.PushBack(&waiter);

    waiters_lock.unlock();

    bool woken;

    if (timeout) {
      woken = waiter.WaitTimed(*timeout);

      {
        // Try to unlink self on timeout
        waiters_lock.lock();
        if (waiter.IsLinked()) {
          waiter.Unlink();
        }
        waiters_lock.unlock();
      }

    } else {
      waiter.Wait();
      woken = true;
    }

    // Re-acquire external mutex
    lock.lock();

    return woken;
  }

 private:
  WaitQueue waiters_;
  ::std::mutex waiters_mutex_;

  // just size_t, we don't expect concurrent wait invocations
  size_t wait_call_count_{0};
};

}  // namespace fault
}  // namespace twist::rt::thr
