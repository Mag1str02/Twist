#pragma once

#include <twist/rt/sim/user/scheduler/interrupt.hpp>

#include <twist/trace/scope.hpp>

#include <cstdlib>
// std::hash
#include <utility>

namespace twist::rt::sim {

namespace user::assist {

// New

template <typename T, typename ... Args>
T* NewImpl(wheels::SourceLocation source_loc, Args&& ... args) {
  {
    // DPOR: artificial dependency for allocations of the same type T
    // (e.g. Node in lock-free stack)
    // void* var = (void*)(typeid(T).hash_code() & 1);

    // TODO: use var addr above
    user::scheduler::Interrupt(source_loc);
  }

  return new T{std::forward<Args>(args)...};
}

template <typename T>
T* New(twist::trace::Scope s = twist::trace::Scope("assist::New")) {
  return NewImpl<T>(s.GetLoc());
}

template <typename T, typename A>
T* New(A&& a, twist::trace::Scope s = twist::trace::Scope("assist::New")) {
  return NewImpl<T>(s.GetLoc(), std::forward<A>(a));
}

template <typename T, typename A, typename B>
T* New(A&& a, B&& b, twist::trace::Scope s = twist::trace::Scope("assist::New")) {
  return NewImpl<T>(s.GetLoc(), std::forward<A>(a), std::forward<B>(b));
}

template <typename T, typename A, typename B, typename C>
T* New(A&& a, B&& b, C&& c, twist::trace::Scope s = twist::trace::Scope("assist::New")) {
  return NewImpl<T>(s.GetLoc(), std::forward<A>(a), std::forward<B>(b), std::forward<C>(c));
}

template <typename T, typename A, typename B, typename C, typename D>
T* New(A&& a, B&& b, C&& c, D&& d, twist::trace::Scope s = twist::trace::Scope("assist::New")) {
  return NewImpl<T>(s.GetLoc(), std::forward<A>(a), std::forward<B>(b), std::forward<C>(c), std::forward<D>(d));
}

template <typename T, typename A, typename B, typename C, typename D, typename E>
T* New(A&& a, B&& b, C&& c, D&& d, E&& e, twist::trace::Scope s = twist::trace::Scope("assist::New")) {
  return NewImpl<T>(s.GetLoc(), std::forward<A>(a), std::forward<A>(a), std::forward<B>(b), std::forward<C>(c), std::forward<D>(d), std::forward<E>(e));
}

// Memory access

void MemoryAccess(void* addr, size_t size, wheels::SourceLocation call_site = wheels::SourceLocation::Current());

// Typed memory access

template <typename T>
void Access(T* ptr, wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
  static_assert(!std::is_void_v<T>, "Expected typed pointer");
  MemoryAccess(ptr, sizeof(T), call_site);
}

// Ptr<T>

template <typename T>
struct Ptr {
  Ptr(T* p = nullptr, wheels::SourceLocation loc = wheels::SourceLocation::Current())
      : raw(p), loc_(loc) {
  }

  // operator T*

  operator T*() {
    return raw;
  }

  operator const T*() const {
    return raw;
  }

  // operator ->

  T* operator->() {
    Access(loc_);
    return raw;
  }

  const T* operator->() const {
    Access(loc_);
    return raw;
  }

  // operator *

  T& operator*() {
    Access(loc_);
    return *raw;
  }

  const T& operator*() const {
    Access(loc_);
    return *raw;
  }

  // operator &

  T** operator&() {
    return &raw;
  }

  T* const * operator&() const {
    return &raw;
  }

  // operator bool

  explicit operator bool() const {
    return raw != nullptr;
  }

  // operator ()

  Ptr<T> operator ()(wheels::SourceLocation call_site = wheels::SourceLocation::Current()) {
    return {raw, call_site};
  }

  // raw pointer

  T* raw;

 private:
  wheels::SourceLocation loc_;

  void Access(wheels::SourceLocation loc) const {
    MemoryAccess(raw, sizeof(T), loc);
  }
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

}  // namespace user::assist

}  // namespace twist::rt::sim
