#pragma once

#include <twist/rt/thr/fault/adversary/inject_fault.hpp>
#include <twist/rt/thr/fault/random/range.hpp>

#include <atomic>
#include <type_traits>

namespace twist::rt::thr {
namespace fault {

// https://en.cppreference.com/w/cpp/atomic/atomic

/////////////////////////////////////////////////////////////////////

template <typename T, bool Int = std::is_integral_v<T> && !std::is_same_v<T, bool>>
class FaultyAtomicBase;

// FaultyAtomicBase for non-integral types

template <typename T>
class FaultyAtomicBase<T, false> {
 public:
  using Impl = ::std::atomic<T>;

 public:
  using value_type = T;  // NOLINT

 public:
  FaultyAtomicBase() noexcept = default;

  FaultyAtomicBase(T value) : impl_(value) {
    AccessAdversary();
  }

  bool is_lock_free() const noexcept {
    return impl_.is_lock_free();
  }

  static constexpr bool is_always_lock_free = Impl::is_always_lock_free;

  // NOLINTNEXTLINE
  T load(std::memory_order mo = std::memory_order::seq_cst) const {
    InjectFaultBefore();
    T value = impl_.load(mo);
    InjectFaultAfter();
    return value;
  }

  operator T() const noexcept {
    return load();
  }

  T DebugLoad(std::memory_order mo = std::memory_order::seq_cst) const {
    return impl_.load(mo);
  }

  // NOLINTNEXTLINE
  void store(T value, std::memory_order mo = std::memory_order::seq_cst) {
    InjectFaultBefore();
    impl_.store(value, mo);
    InjectFaultAfter();
  }

  T operator=(T value) {
    store(value);
    return value;
  }

  T exchange(T new_value, std::memory_order mo = std::memory_order::seq_cst) {
    InjectFaultBefore();
    T prev_value = impl_.exchange(new_value, mo);
    InjectFaultAfter();
    return prev_value;
  }

  // NOLINTNEXTLINE
  bool compare_exchange_weak(T& expected, T desired, std::memory_order success,
                             std::memory_order failure) {
    InjectFaultBefore();

    if (SpuriousFailure()) {
      expected = impl_.load(failure);
      return false;
    }

    bool succeeded =
        impl_.compare_exchange_weak(expected, desired, success, failure);
    InjectFaultAfter();
    return succeeded;
  }

  // NOLINTNEXTLINE
  bool compare_exchange_weak(
      T& expected, T desired,
      std::memory_order mo = std::memory_order::seq_cst) {
    return compare_exchange_weak(expected, desired, mo, mo);
  }

  // NOLINTNEXTLINE
  bool compare_exchange_strong(T& expected, T desired,
                               std::memory_order success,
                               std::memory_order failure) {
    InjectFaultBefore();
    bool succeeded =
        impl_.compare_exchange_strong(expected, desired, success, failure);
    InjectFaultAfter();
    return succeeded;
  }

  // NOLINTNEXTLINE
  bool compare_exchange_strong(
      T& expected, T desired,
      std::memory_order mo = std::memory_order::seq_cst) noexcept {
    return compare_exchange_strong(expected, desired, mo, mo);
  }

#if defined(__TWIST_ATOMIC_WAIT__)

  // NOLINTNEXTLINE
  void wait(T old, std::memory_order mo = std::memory_order::seq_cst) {
    InjectFaultBefore();
    impl_.wait(old, mo);
    InjectFaultAfter();
  }

  // NOLINTNEXTLINE
  void notify_one() {
    InjectFaultBefore();
    impl_.notify_one();
    InjectFaultAfter();
  }

  // NOLINTNEXTLINE
  void notify_all() {
    InjectFaultBefore();
    impl_.notify_all();
    InjectFaultAfter();
  }

#endif

 private:
  static bool SpuriousFailure() {
    return RandomUInteger(11) == 0;
  }

 protected:
  Impl impl_;
};

}  // namespace fault
}  // namespace twist::rt::thr
