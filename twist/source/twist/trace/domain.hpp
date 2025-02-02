#pragma once

#include "attr.hpp"

namespace twist::trace {

class Domain {
 public:
  explicit Domain(const char* name)
      : name_(name) {
  }

  // Non-copyable
  Domain(const Domain&) = delete;
  Domain& operator=(const Domain&) = delete;

  // Non-movable
  Domain(Domain&&) = delete;
  Domain& operator=(Domain&&) = delete;

  const char* GetName() const noexcept {
    return name_;
  }

  Domain& LinkAttr(AttrBase& attr) noexcept {
    attr.next = attr_head_;
    attr_head_ = &attr;
    return *this;
  }

  AttrBase* GetAttrs() const noexcept {
    return attr_head_;
  }

  int GetVisibility() const noexcept {
    return visibility_;
  }

  void SetVisibility(int val) noexcept {
    visibility_ = val;
  }

  static Domain* Global();

 private:
  const char* name_;
  int visibility_ = 1;
  AttrBase* attr_head_ = nullptr;

};

}  // namespace twist::trace
