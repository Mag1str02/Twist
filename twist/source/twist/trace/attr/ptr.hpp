#pragma once

#include "../attr.hpp"

#include "detail/str_buf.hpp"

#include <fmt/format.h>

namespace twist::trace {

namespace attr {

struct PtrAttr : AttrBase {
  const char* name;
  void* ptr;

  PtrAttr(const char* name, void* ptr) {
    this->name = name;
    this->ptr = ptr;
  }

  const char* GetName() const noexcept override {
    return name;
  }

  AttrValue GetValue() const noexcept override {
    return AttrValue::Ptr(ptr);
  }
};

using Ptr = PtrAttr;

}  // namespace attr

}  // namespace twist::trace
