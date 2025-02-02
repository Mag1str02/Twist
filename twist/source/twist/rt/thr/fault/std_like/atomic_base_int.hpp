#pragma once

#include <twist/rt/thr/fault/std_like/atomic_base.hpp>

namespace twist::rt::thr::fault {

// FaultyAtomicBase for integral types

template <typename T>
class FaultyAtomicBase<T, true> : public FaultyAtomicBase<T, false> {
  using Base = FaultyAtomicBase<T, false>;
  using Base::impl_;

 public:
  using difference_type = T;  // NOLINT

 public:
  FaultyAtomicBase()
      : Base() {
  }

  FaultyAtomicBase(T value)
      : Base(value) {
  }

  T operator=(T new_value) {
    store(new_value);
    return new_value;
  }

#define FAULTY_RMW(op)                                                       \
  T op(T value, std::memory_order mo = std::memory_order::seq_cst) { \
    InjectFaultBefore();                                                           \
    T prev_value = impl_.op(value, mo);                                   \
    InjectFaultAfter();                                                           \
    return prev_value;                                                       \
  }

  FAULTY_RMW(fetch_add)
  FAULTY_RMW(fetch_sub)
  FAULTY_RMW(fetch_and)
  FAULTY_RMW(fetch_or)
  FAULTY_RMW(fetch_xor)

#undef FAULTY_RMW

  // Operators

  // Prefix
  T operator++() {
    return fetch_add(1) + T(1);
  }

  // Postfix
  T operator++(int) {
    return fetch_add(1);
  }

  // Prefix
  T operator--() {
    return fetch_sub(1) - T(1);
  }

  // Postfix
  T operator--(int) {
    return fetch_sub(1);
  }

  T operator+=(T delta) {
    return fetch_add(delta) + delta;
  }

  T operator-=(T delta) {
    return fetch_sub(delta) - delta;
  }

  T operator&=(T and_value) {
    return fetch_and(and_value) & and_value;
  }

  T operator|=(T or_value) {
    return fetch_or(or_value) | or_value;
  }

  T operator^=(T xor_value) {
    return fetch_xor(xor_value) ^ xor_value;
  }
};

}  // namespace twist::rt::thr::fault
