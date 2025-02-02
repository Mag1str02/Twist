#pragma once

#include <twist/rt/thr/fault/std_like/atomic_t.hpp>

#include <cassert>

namespace twist::rt::thr::fault {

template <typename T>
class FaultyAtomic<T*> : public FaultyAtomicBase<T*, false> {
  using Base = FaultyAtomicBase<T*>;
  using Base::impl_;

 public:
  using difference_type = std::ptrdiff_t;  // NOLINT

 public:
  FaultyAtomic()
      : Base() {
  }

  FaultyAtomic(T* ptr)
      : Base(ptr) {
    static_assert(sizeof(FaultyAtomic<T*>) == sizeof(T*));
  }

  T* operator=(T* new_ptr) {
    Base::store(new_ptr);
    return new_ptr;
  }

#define FAULTY_FA(op)                                                       \
  T* op(std::ptrdiff_t delta, std::memory_order mo = std::memory_order::seq_cst) { \
    InjectFaultBefore();                                                           \
    T* prev_value = impl_.op(delta, mo);                                   \
    InjectFaultAfter();                                                           \
    return prev_value;                                                       \
  }

  FAULTY_FA(fetch_add)
  FAULTY_FA(fetch_sub)

#undef FAULTY_FA

  // Operators

  // Prefix
  T* operator++() {
    return fetch_add(1) + 1;
  }

  // Postfix
  T* operator++(int) {
    return fetch_add(1);
  }

  // Prefix
  T* operator--() {
    return fetch_sub(1) - 1;
  }

  // Postfix
  T* operator--(int) {
    return fetch_sub(1);
  }

  T* operator+=(std::ptrdiff_t delta) {
    return fetch_add(delta) + delta;
  }

  T* operator-=(std::ptrdiff_t delta) {
    return fetch_sub(delta) - delta;
  }
};

}  // namespace twist::rt::thr::fault
