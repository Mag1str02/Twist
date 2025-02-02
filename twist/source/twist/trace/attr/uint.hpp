#pragma once

#include "../attr.hpp"

#include <cstdint>

namespace twist::trace {

namespace attr {

struct UintAttr : AttrBase {
  const char* name;
  uint64_t value;

  UintAttr(const char* name, uint64_t value) {
    this->name = name;
    this->value = value;
  }

  const char* GetName() const noexcept override {
    return name;
  }

  AttrValue GetValue() const noexcept override {
    return AttrValue::Uint(value);
  }
};

using Uint = UintAttr;

}  // namespace attr

}  // namespace twist::trace
