#pragma once

#include "sharable.hpp"

#include <twist/rt/sim/system/sync/non_atomic.hpp>

#include <twist/rt/sim/user/library/std_like/atomicable.hpp>

#include <twist/rt/sim/system/simulator.hpp>

#include <cassert>

namespace twist::rt::sim {

namespace user::assist {

void NonAtomicInit(void* loc, wheels::SourceLocation source_loc);
void NonAtomicRead(void* loc, wheels::SourceLocation source_loc);
void NonAtomicWrite(void* loc, wheels::SourceLocation source_loc);
void NonAtomicDestroy(void* loc, wheels::SourceLocation source_loc);

template <typename T>
class SharedWriteView {
 public:
  SharedWriteView(T* v, wheels::SourceLocation source_loc)
      : v_(v), source_loc_(source_loc) {
  }

  T& operator*()  {
    Write();
    return *v_;
  }

  T* operator->() {
    Write();
    return v_;
  }

 private:
  void Write() {
    NonAtomicWrite(v_, source_loc_);
  }

 private:
  T* v_;
  wheels::SourceLocation source_loc_;
};

template <typename T>
class SharedReadView {
 public:
  SharedReadView(const T* v, wheels::SourceLocation source_loc)
      : v_(v), source_loc_(source_loc) {
  }

  const T& operator*() const {
    Read();
    return *v_;
  }

  const T* operator->() const {
    Read();
    return v_;
  }

  operator T() const {
    Read();
    return *v_;
  }

 private:
  void Read() const {
    NonAtomicRead((void*)v_, source_loc_);
  }

 private:
  const T* v_;
  wheels::SourceLocation source_loc_;
};

template <typename T>
class Shared {
 public:
  Shared(wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : val_{}, source_loc_(source_loc) {
    Init();
  }

  template <typename A>
  Shared(A&& a, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : val_{std::forward<A>(a)}, source_loc_(source_loc) {
    Init();
  }

  template <typename A, typename B>
  Shared(A&& a, B&& b, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : val_{std::forward<A>(a), std::forward<B>(b)}, source_loc_(source_loc) {
    Init();
  }

  template <typename A, typename B, typename C>
  Shared(A&& a, B&& b, C&& c, wheels::SourceLocation source_loc = wheels::SourceLocation::Current())
      : val_{std::forward<A>(a), std::forward<B>(b), std::forward<C>(c)}, source_loc_(source_loc) {
    Init();
  }

  // Non-copyable
  Shared(const Shared&) = delete;
  Shared& operator=(const Shared&) = delete;

  // Non-movable
  Shared(Shared&&) = delete;
  Shared& operator=(Shared&&) = delete;

  ~Shared() {
    NonAtomicDestroy(Loc(), source_loc_);
  }

  T& operator*() {
    NonAtomicWrite(Loc(), source_loc_);
    return val_;
  }

  const T& operator*() const {
    NonAtomicRead(Loc(), source_loc_);
    return val_;
  }

  operator T() const {
    NonAtomicRead(Loc(), source_loc_);
    return val_;
  }

  T* operator->() {
    NonAtomicWrite(Loc(), source_loc_);
    return &val_;
  }

  const T& operator->() const {
    NonAtomicRead(Loc(), source_loc_);
    return &val_;
  }

  void Write(T new_val, wheels::SourceLocation source_loc = wheels::SourceLocation::Current()) {
    NonAtomicWrite(Loc(), source_loc);
    val_ = std::move(new_val);
  }

  T Read(wheels::SourceLocation source_loc = wheels::SourceLocation::Current()) const {
    NonAtomicRead(Loc(), source_loc);
    return val_;
  }

  SharedReadView<T> ReadView(wheels::SourceLocation source_loc = wheels::SourceLocation::Current()) const {
    return {&val_, source_loc};
  }

  SharedWriteView<T> WriteView(wheels::SourceLocation source_loc = wheels::SourceLocation::Current()) {
    return {&val_, source_loc};
  }

 private:
  void* Loc() const {
    return (void*)&val_;
  }

  void Init() {
    static_assert(Sharable<T>::kStatus);
    NonAtomicInit(&val_, source_loc_);
  };

 private:
  T val_;
  wheels::SourceLocation source_loc_;
};

}  // namespace user::assist

namespace user::library::std_like {

template <typename T>
struct Atomicable<assist::Shared<T>> {
  static const bool kStatus = false;
};

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
