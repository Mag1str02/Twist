#pragma once

#include "manager.hpp"

#include <wheels/core/noncopyable.hpp>

#include <optional>

namespace twist::rt::sim {

namespace user {

template <typename T>
struct TypeDeductionHint {};

template <typename T, typename Init>
class StaticThreadLocalPtr : private tls::VarBase,
                             private wheels::NonCopyable {
 public:
  // NB: Do not allocate memory in ctor!

  StaticThreadLocalPtr(
      TypeDeductionHint<T>,
      const char* name,
      Init init, wheels::SourceLocation loc = wheels::SourceLocation::Current())
      : name_(name),
        loc_(loc),
        init_(init) {
    // No-op
  }

  operator T*() {
    return Load();
  }

  StaticThreadLocalPtr& operator=(T* ptr) {
    Store(ptr);
    return *this;
  }

  // Usage: thread_local_ptr->Method();
  T* operator->() {
    return Load();
  }

  explicit operator bool() const {
    return Load() != nullptr;
  }

  bool operator==(T* ptr) const {
    return Load() == ptr;
  }

  bool operator!=(T* ptr) const {
    return Load() != ptr;
  }

  void _Visit() {
    if (!key_) {
      // First visit

      // Global -> no more visits
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

  // system::tls::IVar

  void* SlotCtor() override {
    return init_();
  }

  void SlotDtor(void* /*ptr*/) override {
    // No-op
  }

  //

  T* Load() const {
    return AccessSlot()->template GetTyped<T>();
  }

  void Store(T* ptr) {
    AccessSlot()->Set(ptr);
  }

  tls::Slot* AccessSlot() const {
    return tls::Manager::Instance().Access(*key_);
  }

 private:
  const char* name_;
  wheels::SourceLocation loc_;
  std::optional<tls::Key> key_;
  Init init_;
};

}  // namespace user

}  // namespace twist::rt::sim
