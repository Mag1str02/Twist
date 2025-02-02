#pragma once

#include "mutex_traits.hpp"

#include <twist/trace/scope.hpp>

namespace twist::rt::sim {

namespace user::library::std_like {

template <typename MutexType, bool Native>
class LockGuardImpl;

// Native

template <typename MutexType>
class LockGuardImpl<MutexType, /*Native=*/true> {
 public:
  LockGuardImpl(MutexType& mutex, wheels::SourceLocation source_loc)
      : mutex_(&mutex),
        source_loc_(source_loc) {
    mutex_->lock(source_loc);
  }

  ~LockGuardImpl() {
    mutex_->unlock(source_loc_);
  }

 private:
  MutexType* mutex_;
  wheels::SourceLocation source_loc_;
};

// User

template <typename MutexType>
class LockGuardImpl<MutexType, /*Native=*/false> {
 public:
  LockGuardImpl(MutexType& mutex, wheels::SourceLocation /*source_loc*/)
      : locker_{mutex} {
    //
  }

  ~LockGuardImpl() {
    //
  }

 private:
  std::lock_guard<MutexType> locker_;
};

// Facade

template <typename MutexType>
class lock_guard {  // NOLINT
  using Impl = LockGuardImpl<MutexType, MutexTraits<MutexType>::kNative>;

 public:
  lock_guard(MutexType& mutex, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : impl_(mutex, source_loc) {
  }

  // Non-copyable
  lock_guard(const lock_guard&) = delete;
  lock_guard& operator=(const lock_guard&) = delete;

  // Non-movable
  lock_guard(lock_guard&&) = delete;
  lock_guard& operator=(lock_guard&&) = delete;

  ~lock_guard() {
  }

 private:
  Impl impl_;
};

}  // user::library::std_like

}  // namespace twist::rt::sim
