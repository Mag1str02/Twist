#pragma once

#include <twist/rt/thr/fault/std_like/atomic_base.hpp>

#include <cassert>

namespace twist::rt::thr::fault {

template <typename T>
class FaultyAtomic : public FaultyAtomicBase<T> {
  using Base = FaultyAtomicBase<T>;

 public:
  FaultyAtomic()
      : Base() {
  }

  FaultyAtomic(T value)
      : Base(value) {
    static_assert(sizeof(FaultyAtomic<T>) == sizeof(T));
  }

  T operator=(T new_value) {
    Base::store(new_value);
    return new_value;
  }
};

}  // namespace twist::rt::thr::fault
