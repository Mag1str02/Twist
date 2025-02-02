#pragma once

#include "../attr.hpp"

#include <cstdint>

namespace twist::trace {

namespace attr {

struct BoolAttr : AttrBase {
  const char* name;
  bool flag;

  BoolAttr(const char* name, bool flag) {
    this->name = name;
    this->flag = flag;
  }

  const char* GetName() const noexcept override {
    return name;
  }

  AttrValue GetValue() const noexcept override {
    return AttrValue::Bool(flag);
  }
};

using Bool = BoolAttr;

}  // namespace attr

}  // namespace twist::trace
