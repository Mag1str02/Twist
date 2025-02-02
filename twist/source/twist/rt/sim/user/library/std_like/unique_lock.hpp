#pragma once

#include "mutex_traits.hpp"

#include <mutex>

#include <twist/trace/scope.hpp>

namespace twist::rt::sim {

namespace user::library::std_like {

template <typename MutexType, bool Native>
class UniqueLockImpl;

// Native

// Capture and forward source location

template <typename MutexType>
class UniqueLockImpl<MutexType, /*Native=*/true> {
 public:
  UniqueLockImpl(MutexType& mutex, wheels::SourceLocation source_loc)
      : mutex_(&mutex),
        owns_(true),
        source_loc_(source_loc) {
    mutex_->lock(source_loc_);
  }

  UniqueLockImpl(MutexType& mutex, std::defer_lock_t, wheels::SourceLocation source_loc)
      : mutex_(&mutex),
        owns_(false),
        source_loc_(source_loc) {
    //
  }

  UniqueLockImpl(MutexType& mutex, std::adopt_lock_t, wheels::SourceLocation source_loc)
      : mutex_(&mutex),
        owns_(true),
        source_loc_(source_loc) {
    //
  }

  UniqueLockImpl(MutexType& mutex, std::try_to_lock_t, wheels::SourceLocation source_loc)
      : mutex_(&mutex),
        source_loc_(source_loc) {
    owns_ = mutex_->try_lock(source_loc);
  }

  ~UniqueLockImpl() {
    if (owns_) {
      mutex_->unlock(source_loc_);
    }
  }

  // Movable

  UniqueLockImpl(UniqueLockImpl&& that)
      : mutex_(that.mutex_),
        owns_(that.owns_),
        source_loc_(that.source_loc_) {  // TODO: Source location?
    that.mutex_ = nullptr;
    that.owns_ = false;
  }

  // Move-assignable

  UniqueLockImpl& operator=(UniqueLockImpl&& that) {
    if (owns_) {
      mutex_->unlock(source_loc_);
    }

    mutex_ = that.mutex_;
    owns_ = that.owns_;

    source_loc_ = that.source_loc_;

    that.mutex_ = nullptr;
    that.owns_ = false;

    return *this;
  }

  // Non-copyable

  UniqueLockImpl(const UniqueLockImpl&) = delete;
  UniqueLockImpl& operator=(const UniqueLockImpl&) = delete;

  // Locking

  void Lock(wheels::SourceLocation call_site) {
    mutex_->lock(call_site);
    owns_ = true;
  }

  bool TryLock(wheels::SourceLocation call_site) {
    owns_ = mutex_->try_lock(call_site);
    return owns_;
  }

  void Unlock(wheels::SourceLocation call_site) {
    mutex_->unlock(call_site);
    owns_ = false;
  }

  // Observers

  MutexType* Mutex() const noexcept {
    return mutex_;
  }

  bool OwnsLock() const noexcept {
    return owns_;
  }

  // Modifiers

  MutexType* Release() noexcept {
    MutexType* mutex = std::exchange(mutex_, nullptr);
    owns_ = false;
    return mutex;
  }

  void Swap(UniqueLockImpl& that) noexcept {
    std::swap(mutex_, that.mutex_);
    std::swap(owns_, that.owns_);
    // std::swap(source_loc_, that.source_loc_);
  }

 private:
  MutexType* mutex_;
  bool owns_;
  // TODO: Last lock site
  wheels::SourceLocation source_loc_;
};

// User

template <typename MutexType>
class UniqueLockImpl<MutexType, /*Native=*/false> {
 public:
  UniqueLockImpl(MutexType& mutex, wheels::SourceLocation /*source_loc*/)
      : locker_(mutex) {
  }

  UniqueLockImpl(MutexType& mutex, std::defer_lock_t, wheels::SourceLocation /*source_loc*/)
      : locker_(mutex, std::defer_lock) {
  }

  UniqueLockImpl(MutexType& mutex, std::adopt_lock_t, wheels::SourceLocation /*source_loc*/)
      : locker_(mutex, std::adopt_lock) {
  }

  UniqueLockImpl(MutexType& mutex, std::try_to_lock_t, wheels::SourceLocation /*source_loc*/)
      : locker_(mutex, std::try_to_lock) {
  }

  // Movable

  UniqueLockImpl(UniqueLockImpl&& that)
      : locker_(std::move(that.locker_)) {
  }

  UniqueLockImpl& operator=(UniqueLockImpl&& that) {
    locker_ = std::move(that.locker_);
    return *this;
  }

  // Non-copyable

  UniqueLockImpl(const UniqueLockImpl&) = delete;
  UniqueLockImpl& operator=(const UniqueLockImpl&) = delete;

  // Locking

  void Lock(wheels::SourceLocation /*call_site*/) {
    locker_.lock();

  }

  bool TryLock(wheels::SourceLocation /*call_site*/) {
    return locker_.try_lock();
  }

  void Unlock(wheels::SourceLocation /*call_site*/) {
    locker_.unlock();
  }

  // Observers

  MutexType* Mutex() const noexcept {
    return locker_.mutex();
  }

  bool OwnsLock() const noexcept {
    return locker_.owns_lock();
  }

  // Modifiers

  MutexType* Release() noexcept {
    return locker_.release();
  }

  void Swap(UniqueLockImpl& that) {
    locker_.swap(that.locker_);
  }

 private:
  std::unique_lock<MutexType> locker_;
};

// Facade

template <typename MutexType>
class unique_lock {  // NOLINT
  using Impl = UniqueLockImpl<MutexType, MutexTraits<MutexType>::kNative>;

 public:
  unique_lock(MutexType& mutex, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : impl_(mutex, source_loc) {
  }

  unique_lock(MutexType& mutex, std::defer_lock_t, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : impl_(mutex, std::defer_lock, source_loc) {
  }

  unique_lock(MutexType& mutex, std::adopt_lock_t, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : impl_(mutex, std::adopt_lock, source_loc) {
  }

  unique_lock(MutexType& mutex, std::try_to_lock_t, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : impl_(mutex, std::try_to_lock, source_loc) {
  }

  // Non-copyable
  unique_lock(const unique_lock&) = delete;
  unique_lock& operator=(const unique_lock&) = delete;

  // Movable
  unique_lock(unique_lock&&) = default;
  unique_lock& operator=(unique_lock&&) = default;

  ~unique_lock() {
    //
  }

  // Locking

  void lock(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
    impl_.Lock(call_site);
  }

  bool try_lock(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
    return impl_.TryLock(call_site);
  }

  void unlock(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
    impl_.Unlock(call_site);
  }

  // Observers

  MutexType* mutex() {
    return impl_.Mutex();
  }

  bool owns_lock() const {
    return impl_.OwnsLock();
  }

  explicit operator bool() const {
    return owns_lock();
  }

  // Modifiers

  MutexType* release() {
    return impl_.Release();
  }

  void swap(unique_lock& that) {
    impl_.Swap(that.impl_);
  }

 private:
  Impl impl_;
};

}  // user::library::std_like

}  // namespace twist::rt::sim
