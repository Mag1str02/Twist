#pragma once

namespace wheels {

/*
 * Usage:
 * class X : private wheels::NonCopyable { ... }
 */

struct NonCopyable {
  NonCopyable(const NonCopyable& that) = delete;
  NonCopyable& operator=(const NonCopyable& that) = delete;

 protected:
  // To be used only as a base class
  NonCopyable() = default;
  ~NonCopyable() = default;
};

}  // namespace wheels
