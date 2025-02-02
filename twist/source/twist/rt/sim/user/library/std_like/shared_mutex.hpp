#pragma once

#include "sync_var.hpp"
#include "mutex_owner.hpp"
#include "mutex_traits.hpp"

#include <twist/trace/scope.hpp>
#include <twist/trace/attr/unit.hpp>

// std::lock_guard, std::unique_lock
#include <mutex>

namespace twist::rt::sim {

namespace user::library::std_like {

// Tracing

namespace domain {

// Static domains
trace::Domain& SharedMutex();
trace::Domain& SharedMutexImpl();

}  // namespace domain

//

class SharedMutexImpl {
 public:
  explicit SharedMutexImpl(wheels::SourceLocation source_loc)
      : state_(source_loc) {
  }

  void Init(wheels::SourceLocation source_loc) {
    trace::Scope init{domain::SharedMutexImpl()};

    state_.Init(Pack(false, 0), source_loc);
  }

  void Destroy() {
    trace::Scope destroy{domain::SharedMutexImpl()};

    state_.Destroy();
  }

  // Writer

  void Lock(wheels::SourceLocation call_site) {
    trace::Scope lock{domain::SharedMutexImpl()};

    while (true) {
      uint32_t state = state_.Load(std::memory_order::acquire, call_site);
      auto [writer, readers] = Unpack(state);
      if (!writer && (readers == 0)) {
        if (state_.CompareExchangeStrong(state, Pack(true, 0), std::memory_order::relaxed, std::memory_order::relaxed, call_site)) {
          break;
        }
      } else {
        system::WaiterContext waiter{system::FutexType::MutexLock, "shared_mutex::lock", call_site};
        syscall::FutexWait(state_.FutexLoc(), state, &waiter);
      }
    }
  }

  bool TryLock(wheels::SourceLocation call_site) {
    trace::Scope try_lock{domain::SharedMutexImpl()};

    uint32_t unlocked = Pack(false, 0);
    if (state_.CompareExchangeWeak(unlocked, Pack(true, 0), std::memory_order::acquire, std::memory_order_relaxed, call_site)) {
      return true;
    } else {
      return false;
    }
  }

  void Unlock(wheels::SourceLocation call_site) {
    trace::Scope unlock{domain::SharedMutexImpl()};

    auto futex = state_.FutexLoc();
    state_.Store(Pack(false, 0), std::memory_order::release, call_site);
    system::WakerContext waker{"shared_mutex::unlock", call_site};
    syscall::FutexWake(futex, 0, &waker);  // All
  }

  // Reader

  void LockShared(wheels::SourceLocation call_site) {
    trace::Scope lock_shared{domain::SharedMutexImpl()};

    while (true) {
      uint32_t state = state_.Load(std::memory_order::acquire, call_site);
      auto [writer, readers] = Unpack(state);
      if (!writer) {
        if (state_.CompareExchangeStrong(state, Pack(false, readers + 1), std::memory_order::relaxed, std::memory_order::relaxed, call_site)) {
          break;
        }
      } else {
        system::WaiterContext waiter{system::FutexType::MutexLock, "shared_mutex::lock_shared", call_site};
        syscall::FutexWait(state_.FutexLoc(), state, &waiter);
      }
    }
  }

  bool TryLockShared(wheels::SourceLocation call_site) {
    trace::Scope try_lock_shared{domain::SharedMutexImpl()};

    while (true) {
      uint32_t state = state_.Load(std::memory_order::acquire, call_site);
      auto [writer, readers] = Unpack(state);

      if (!writer) {
        if (state_.CompareExchangeStrong(state, Pack(false, readers + 1), std::memory_order::relaxed, std::memory_order::relaxed, call_site)) {
          return true;
        }
      } else {
        return false;
      }
    }
  }

  void UnlockShared(wheels::SourceLocation call_site) {
    trace::Scope unlock_shared{domain::SharedMutexImpl()};

    uint32_t old_state = state_.FetchSub(2, std::memory_order::release, call_site);
    auto [writer, readers] = Unpack(old_state);

    ___TWIST_SIM_USER_VERIFY(readers > 0, "Expected std::shared_mutex in readers state");

    if (readers == 1) {
      // Last reader
      system::WakerContext waker{"shared_mutex::unlock_shared", call_site};
      syscall::FutexWake(state_.FutexLoc(), 1, &waker);  // One
    }
  }

 private:
  static std::pair<bool, uint32_t> Unpack(uint32_t state) {
    bool writer = (state & 1) == 1;
    uint32_t readers = state >> 1;
    return {writer, readers};
  }

  static uint32_t Pack(bool writer, uint32_t readers) {
    return (readers << 1) | (writer ? 1 : 0);
  }

 private:
  SyncVar state_;  // writer bit + (readers << 1);
};


class shared_mutex {
 public:
  shared_mutex(wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : impl_(source_loc) {
    {
      trace::Scope ctor{domain::SharedMutex(), "shared_mutex", source_loc};

      impl_.Init(source_loc);

      ctor.Note("shared_mutex::shared_mutex", trace::NoteCtx{source_loc});
    }
  }

  ~shared_mutex() {
    trace::Scope dtor{domain::SharedMutex(), "~shared_mutex"};

    impl_.Destroy();
  }

  // Non-copyable
  shared_mutex(const shared_mutex&) = delete;
  shared_mutex& operator=(const shared_mutex&) = delete;

  // Non-movable
  shared_mutex(shared_mutex&&) = delete;
  shared_mutex& operator=(shared_mutex&&) = delete;

  // Writer

  void lock(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope lock{domain::SharedMutex(), "lock", call_site};

    lock.NoteInline("Enter lock");

    impl_.Lock(call_site);
    owner_.Lock();

    lock.NoteInline("Mutex acquired");
  }

  bool try_lock(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope try_lock{domain::SharedMutex(), "try_lock", call_site};

    bool acquired = impl_.TryLock(call_site);
    if (acquired) {
      owner_.Lock();
    }

    try_lock.NoteInline("try_lock() -> {}", acquired);

    return acquired;
  }

  void unlock(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope unlock{domain::SharedMutex(), "unlock", call_site};

    unlock.NoteInline("Enter unlock");

    owner_.Unlock();
    impl_.Unlock(call_site);

    unlock.NoteInline("Mutex released");
  }

  // Reader

  void lock_shared(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope lock_shared{domain::SharedMutex(), "lock_shared", call_site};

    lock_shared.NoteInline("Enter lock_shared");

    impl_.LockShared(call_site);

    lock_shared.NoteInline("Exit lock_shared");
  }

  bool try_lock_shared(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope try_lock_shared{domain::SharedMutex(), "try_lock_shared", call_site};

    bool acquired = impl_.TryLockShared(call_site);

    try_lock_shared.NoteInline("try_lock_shared() -> {}", acquired);

    return acquired;
  }

  void unlock_shared(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {  // NOLINT
    trace::Scope unlock_shared{domain::SharedMutex(), "unlock_shared", call_site};

    unlock_shared.NoteInline("Enter unlock_shared");

    impl_.UnlockShared(call_site);

    unlock_shared.NoteInline("Exit unlock_shared");
  }

 private:
  SharedMutexImpl impl_;
  MutexOwner owner_;  // writer
};

template <>
struct MutexTraits<shared_mutex> {
  static constexpr bool kNative = true;
};

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
