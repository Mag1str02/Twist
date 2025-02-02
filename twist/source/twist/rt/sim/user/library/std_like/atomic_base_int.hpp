#pragma once

#include "atomic_base.hpp"

#include <twist/rt/sim/user/scheduler/interrupt.hpp>

namespace twist::rt::sim {

namespace user::library::std_like {

// AtomicBase for integral types

template <typename T>
class AtomicBase<T, true> : public AtomicBase<T, false> {
  using Base = AtomicBase<T, false>;

 public:
  using difference_type = T;  // NOLINT

 public:
  AtomicBase(wheels::SourceLocation source_loc)
      : Base(source_loc) {
  }

  T operator=(T new_value) noexcept {
    Base::store(new_value);
    return new_value;
  }

  // NOLINTNEXTLINE
  T fetch_add(T delta, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    return RMW([delta](T curr_value) {
      return curr_value + delta;
    }, mo, call_site, "atomic::fetch_add");
  }

  // NOLINTNEXTLINE
  T fetch_sub(T delta, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    return RMW([delta](T curr_value) {
      return curr_value - delta;
    }, mo, call_site, "atomic::fetch_sub");
  }

  // NOLINTNEXTLINE
  T fetch_and(T and_value, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    return RMW([and_value](T curr_value) {
      return curr_value & and_value;
    }, mo, call_site, "atomic::fetch_and");
  }

  // NOLINTNEXTLINE
  T fetch_or(T or_value, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    return RMW([or_value](T curr_value) {
      return curr_value | or_value;
    }, mo, call_site, "atomic::fetch_or");
  }

  // NOLINTNEXTLINE
  T fetch_xor(T xor_value, std::memory_order mo = std::memory_order::seq_cst, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) noexcept {
    return RMW([xor_value](T curr_value) {
      return curr_value ^ xor_value;
    }, mo, call_site, "atomic::fetch_xor");
  }

  // Operators

  // Prefix
  T operator++() noexcept {
    return fetch_add(1) + T(1);
  }

  // Postfix
  T operator++(int) noexcept {
    return fetch_add(1);
  }

  // Prefix
  T operator--() noexcept {
    return fetch_sub(1) - T(1);
  }

  // Postfix
  T operator--(int) noexcept {
    return fetch_sub(1);
  }

  T operator+=(T delta) noexcept {
    return fetch_add(delta) + delta;
  }

  T operator-=(T delta) noexcept {
    return fetch_sub(delta) - delta;
  }

  T operator&=(T and_value) noexcept {
    return fetch_and(and_value) & and_value;
  }

  T operator|=(T or_value) noexcept {
    return fetch_or(or_value) | or_value;
  }

  T operator^=(T xor_value) noexcept {
    return fetch_xor(xor_value) ^ xor_value;
  }

 private:
  template <typename F>
  T RMW(F modify, std::memory_order mo, wheels::SourceLocation call_site, const char* op) noexcept {
    system::sync::Action load{this, Base::Size(), system::sync::ActionType::AtomicRmwLoad, 0, mo,
                              Base::ValueType(), op, call_site};
    uint64_t r = syscall::Sync(&load);

    T old_value = Base::ToValue(r);
    T new_value = modify(old_value);

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

}  // namespace twist::rt::sim
