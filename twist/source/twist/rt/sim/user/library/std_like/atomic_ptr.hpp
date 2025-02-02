#pragma once

#include "atomic_t.hpp"

#include <twist/rt/sim/user/assist/sharable.hpp>

#include <cassert>
// std::ptrdiff_t
#include <cstddef>

namespace twist::rt::sim {

namespace user::library::std_like {

template <typename T>
class atomic<T*> : public AtomicBase<T*, false> {
  using Base = AtomicBase<T*, false>;

 public:
  using difference_type = std::ptrdiff_t;  // NOLINT

 public:
  [[deprecated("Atomic default constructor is error-prone. Please consider explicit initialization")]] atomic() noexcept(std::is_nothrow_default_constructible_v<T>) {
    static_assert(std::is_default_constructible<T>());
  }

  atomic(T* ptr, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : Base(source_loc) {
    static_assert(sizeof(atomic<T*>) == sizeof(T*));
    Base::Init(ptr, source_loc);
  }

  ~atomic() {
    Base::Destroy();
  }

  T* operator=(T* new_ptr) noexcept {
    Base::store(new_ptr);
    return new_ptr;
  }

  // NOLINTNEXTLINE
  T* fetch_add(std::ptrdiff_t delta, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    return FetchArithm(delta, mo, "atomic::fetch_add", call_site);
  }

  // NOLINTNEXTLINE
  T* fetch_sub(std::ptrdiff_t delta, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    return FetchArithm(-delta, mo, "atomic::fetch_sub", call_site);
  }

  // Operators

  // Prefix
  T* operator++() noexcept {
    return fetch_add(1) + 1;
  }

  // Postfix
  T* operator++(int) noexcept {
    return fetch_add(1);
  }

  // Prefix
  T* operator--() noexcept {
    return fetch_sub(1) - 1;
  }

  // Postfix
  T* operator--(int) noexcept {
    return fetch_sub(1);
  }

  T* operator+=(std::ptrdiff_t delta) noexcept {
    return fetch_add(delta) + delta;
  }

  T* operator-=(std::ptrdiff_t delta) noexcept {
    return fetch_sub(delta) - delta;
  }

 private:
  T* FetchArithm(std::ptrdiff_t delta, std::memory_order mo, const char* op, wheels::SourceLocation call_site) noexcept {
    system::sync::Action load{this, Base::Size(), system::sync::ActionType::AtomicRmwLoad, 0, mo,
                              Base::ValueType(), op, call_site};
    uint64_t r = syscall::Sync(&load);

    T* old_value = Base::ToValue(r);
    T* new_value = old_value + delta;

    {
      scheduler::PreemptionGuard g;
      system::sync::Action store{this, Base::Size(), system::sync::ActionType::AtomicRmwCommit, Base::ToRepr(new_value), mo,
                                 Base::ValueType(), op, call_site};
      syscall::Sync(&store);

      Base::StoreDebug(new_value);
    }

    return old_value;
  }
};

}  // namespace user::library::std_like

namespace user::assist {

template <typename T>
struct Sharable<library::std_like::atomic<T*>> {
  static const bool kStatus = false;
};

}  // namespace user::assist

}  // namespace twist::rt::sim
