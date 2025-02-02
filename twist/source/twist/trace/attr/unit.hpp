#pragma once

#include "../attr.hpp"

#include <cstdint>

namespace twist::trace {

namespace attr {

struct UnitAttr : AttrBase {
  const char* name;

  explicit UnitAttr(const char* name) {
    this->name = name;
  }

  const char* GetName() const noexcept override {
    return name;
  }

  AttrValue GetValue() const noexcept override {
    return AttrValue::Unit();
  }
};

using Unit = UnitAttr;
using Tag = UnitAttr;

}  // namespace attr

}  // namespace twist::trace
