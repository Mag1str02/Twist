#pragma once

#include "manager.hpp"

#include <twist/rt/sim/user/am_i_user.hpp>

#include <wheels/core/assert.hpp>
#include <wheels/core/noncopyable.hpp>

namespace twist::rt::sim {

namespace user {

template <typename T>
class StaticVar : private ss::VarBase,
                  private wheels::NonCopyable {
 public:
  StaticVar(
      const char* name,
      bool member = false,
      wheels::SourceLocation loc = wheels::SourceLocation::Current())
      : name_(name),
        loc_(loc) {
    if (member) {
      // Workaround for members
      _Visit();
    }
  }

  T* Ptr() {
    WHEELS_ASSERT(var_ != nullptr, "Accessing non-initialized static variable");
    return var_;
  }

  T& Ref() {
    return *Ptr();
  }

  T& operator*() {
    return Ref();
  }

  T* operator->() {
    return Ptr();
  }

  T* operator&() {
    return Ptr();
  }

  void _Visit() {
    if (var_ == nullptr) {
      // First visit

      // Global -> no more __Visit-s
      // Local -> Ctor -> var_
      ss::Manager::Instance().Register(this);
    }
  }

 private:
  // ss::IVar

  size_t Size() const override {
    return sizeof(T);
  }

  void Ctor(void* addr) override {
    WHEELS_ASSERT(user::AmIUser(), "User thread expected");
    WHEELS_ASSERT(var_ == nullptr, "Static var already initialized");
    var_ = new (addr) T{};
  }

  void Dtor() override {
    WHEELS_ASSERT(user::AmIUser(), "User thread expected");
    WHEELS_ASSERT(var_ != nullptr, "Static var not initialized");
    T* var = std::exchange(var_, nullptr);
    var->~T();
  }

  const char* Name() const override {
    return name_;
  }

  wheels::SourceLocation SourceLoc() const override {
    return loc_;
  }

  int InitOrder() const override {
    return 1;
  }

  // Isolated memory
  void Burn() override {
    var_ = nullptr;
  }

 private:
  const char* name_;
  wheels::SourceLocation loc_;
  T* var_;
};

}  // namespace user

}  // namespace twist::rt::sim
