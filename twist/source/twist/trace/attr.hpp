#pragma once

#include "attr_value.hpp"

namespace twist::trace {

struct IAttr {
  virtual const char* GetName() const noexcept = 0;
  virtual AttrValue GetValue() const noexcept = 0;
};

struct AttrBase : IAttr {
  AttrBase* next;
};

}  // namespace twist::trace
