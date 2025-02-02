#pragma once

#include "atomic_base.hpp"
#include "atomic_base_int.hpp"

#include <twist/rt/sim/user/assist/sharable.hpp>

#include <cassert>

namespace twist::rt::sim {

namespace user::library::std_like {

template <typename T>
struct AtomicTypeCheck {
  static constexpr bool FitsInArchWord() {
    // Assuming 64-bit architecture
    return sizeof(T) <= sizeof(std::uint64_t);
  }

  static_assert(FitsInArchWord(), "Atomic value type is not supported for performance reasons");
  static_assert(std::is_trivially_copyable<T>::value, "atomic<T> requires that 'T' be a trivially copyable type");
};

template <typename T>
class atomic : public AtomicBase<T>,
               private AtomicTypeCheck<T> {
  using Base = AtomicBase<T>;

 public:
  [[deprecated("Atomic default constructor is error-prone. Please consider explicit initialization")]] atomic() noexcept(std::is_nothrow_default_constructible_v<T>) {
    static_assert(std::is_default_constructible<T>());
  }

  atomic(T value, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : Base(source_loc) {
    static_assert(sizeof(atomic<T>) == sizeof(T));
    Base::Init(value, source_loc);
  }

  ~atomic() {
    Base::Destroy();
  }

  T operator=(T new_value) noexcept {
    Base::store(new_value);
    return new_value;
  }
};

}  // namespace user::library::std_like

namespace user::assist {

template <typename T>
struct Sharable<library::std_like::atomic<T>> {
  static const bool kStatus = false;
};

}  // namespace user::assist

}  // namespace twist::rt::sim
