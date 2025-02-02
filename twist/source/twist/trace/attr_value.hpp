#pragma once

#include <cstdint>
#include <variant>
#include <string_view>

#include <wheels/core/compiler.hpp>

namespace twist::trace {

enum class AttrValueType {
  Uint = 0,
  Ptr = 1,
  Bool = 2,
  Str = 3,
  Unit = 4
};

class AttrValue {
  // clang-format off
  using Storage = std::variant<
      uint64_t,
      void*,
      bool,
      std::string_view,
      std::monostate
      >;
  // clang-format on

 public:
  template <typename T>
  AttrValue(T value)
      : store_{value} {
  }

  static AttrValue Uint(uint64_t value) {
    return AttrValue{value};
  }

  static AttrValue Ptr(void* ptr) {
    return AttrValue{ptr};
  }

  static AttrValue Bool(bool flag) {
    return AttrValue{flag};
  }

  static AttrValue Str(std::string_view view) {
    return AttrValue{view};
  }

  static AttrValue Unit() {
    return AttrValue(std::monostate{});
  }

  AttrValueType ValueType() const {
    switch (store_.index()) {
      case 0:
        return AttrValueType::Uint;
      case 1:
        return AttrValueType::Ptr;
      case 2:
        return AttrValueType::Bool;
      case 3:
        return AttrValueType::Str;
      case 4:
        return AttrValueType::Unit;
      default:
        WHEELS_UNREACHABLE();
    }
  }

  uint64_t GetUint() const {
    return std::get<uint64_t>(store_);
  }

  void* GetPtr() const {
    return std::get<void*>(store_);
  }

  bool GetBool() const {
    return std::get<bool>(store_);
  }

  std::string_view GetStr() const {
    return std::get<std::string_view>(store_);
  }

 private:
  Storage store_;
};

}  // namespace twist::trace
