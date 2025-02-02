#pragma once

#include "manager.hpp"

#include <wheels/core/noncopyable.hpp>

#include <cassert>
#include <optional>
#include <type_traits>

namespace twist::rt::sim {

namespace user {

template <typename T>
class StaticThreadLocalVar : private tls::VarBase,
                             private wheels::NonCopyable {
 public:
  // NB: Do not allocate memory in ctor!
  StaticThreadLocalVar(
      const char* name,
      bool member = false,
      wheels::SourceLocation loc = wheels::SourceLocation::Current())
      : name_(name),
        loc_(loc) {

    static_assert(!std::is_pointer_v<T>, "Consider TWISTED_STATIC_THREAD_LOCAL_PTR macro for static thread_local pointer declaration");

    if (member) {
      _Visit();
    }
  }

  T& operator*() {
    return *Ptr();
  }

  T* operator->() {
    return Ptr();
  }

  T* operator&() {
    return Ptr();
  }

  void operator=(T v) {
    this->operator*() = std::move(v);
  }

  void _Visit() {
    if (!key_) {
      // Global -> no more __Visit-s
      // Local -> Ctor -> key_
      ss::Manager::Instance().Register(this);
    }

    if (key_) {
      // Definitely local -> Maybe init if first access from current thread
      tls::Manager::Instance().Visit(*key_);
    }
  }

 private:
  // ss::IVar

  size_t Size() const override {
    return 1;
  }

  void Ctor(void* /*addr*/) override {
    key_ = tls::Manager::Instance().Register(this);
  }

  void Dtor() override {
    key_.reset();
  }

  const char* Name() const override {
    return name_;
  }

  wheels::SourceLocation SourceLoc() const override {
    return loc_;
  }

  int InitOrder() const override {
    return 2;
  }

  // Isolated memory
  void Burn() override {
    key_.reset();
  }

  // tls::IVar

  void* SlotCtor() override {
    return new T{};
  }

  void SlotDtor(void* ptr) override {
    delete (T*)ptr;
  }

  //

  T* Ptr() {
    tls::Slot* slot = tls::Manager::Instance().Access(*key_);
    return slot->GetTyped<T>();
  }

 private:
  const char* name_;
  wheels::SourceLocation loc_;
  std::optional<tls::Key> key_;
};

}  // namespace user

}  // namespace twist::rt::sim
