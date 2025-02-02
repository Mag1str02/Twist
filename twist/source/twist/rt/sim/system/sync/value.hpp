#pragma once

#include <cstdint>
#include <type_traits>

namespace twist::rt::sim {

namespace system::sync {

enum ValueType : uint32_t {
  Bool,
  Int,
  Ptr,
  None,
  Some,
};

template <typename T>
ValueType GetValueType() {
  if constexpr (std::is_same_v<T, bool>) {
    return ValueType::Bool;
  } else if constexpr (std::is_integral_v<T>) {
    return ValueType::Int;
  } else if constexpr (std::is_pointer_v<T>) {
    return ValueType::Ptr;
  } else {
    return ValueType::Some;
  }
}

}  // namespace system::sync

}  // namespace twist::rt::sim
