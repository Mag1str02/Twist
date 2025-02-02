#pragma once

#include <wheels/core/panic.hpp>

namespace twist::rt::thr {

namespace assist {

// New

template <typename T, typename ... Args>
T* New(Args&& ... args) {
  return new T{std::forward<Args>(args)...};
}

// Memory access

void MemoryAccess(void* addr, size_t) {
  if (addr == nullptr) {
    wheels::Panic("Nullptr access");
  }
  // TODO: AddressSanitizer
}

// Typed memory access

template <typename T>
void Access(T* ptr) {
  static_assert(!std::is_void_v<T>, "Expected typed pointer");
  MemoryAccess(ptr, sizeof(T));
}

// Ptr<T>

template <typename T>
struct Ptr {
  Ptr(T* p) {
    raw = p;
  }

  // operator T*

  operator T*() noexcept {
    return raw;
  }

  operator const T*() const noexcept {
    return raw;
  }

  // operator ->

  T* operator->() noexcept {
    return raw;
  }

  const T* operator->() const noexcept {
    return raw;
  }

  // operator *

  T& operator*() noexcept {
    return *raw;
  }

  const T& operator*() const noexcept {
    return *raw;
  }

  // operator &

  T** operator&() noexcept {
    return &raw;
  }

  T* const * operator&() const noexcept {
    return &raw;
  }

  // operator bool

  explicit operator bool() const noexcept {
    return raw != nullptr;
  }

  // operator ()

  Ptr<T> operator ()() {
    return {raw};
  }

  // raw pointer

  T* raw;
};

// ==

template <typename T>
bool operator==(Ptr<T> lhs, Ptr<T> rhs) {
  return lhs.raw == rhs.raw;
}

template <typename T>
bool operator==(Ptr<T> lhs, T* rhs) {
  return lhs.raw == rhs;
}

template <typename T>
bool operator==(T* lhs, Ptr<T> rhs) {
  return lhs == rhs.raw;
}

// !=

template <typename T>
bool operator!=(Ptr<T> lhs, Ptr<T> rhs) {
  return lhs.raw != rhs.raw;
}

template <typename T>
bool operator!=(Ptr<T> lhs, T* rhs) {
  return lhs.raw != rhs;
}

template <typename T>
bool operator!=(T* lhs, Ptr<T> rhs) {
  return lhs != rhs.raw;
}

// TODO: more operators

}  // namespace assist

}  // namespace twist::rt::thr
