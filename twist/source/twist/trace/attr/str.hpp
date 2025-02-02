#pragma once

#include "../attr.hpp"

namespace twist::trace {

namespace attr {

struct StrAttr : AttrBase {
  const char* name;
  const char* value;

  StrAttr(const char* name, const char* value) {
    this->name = name;
    this->value = value;
  }

  const char* GetName() const noexcept override {
    return name;
  }

  AttrValue GetValue() const noexcept override {
    return AttrValue::Str(value);
  }
};

using Str = StrAttr;

}  // namespace attr

}  // namespace twist::trace
