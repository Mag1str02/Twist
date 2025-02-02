#pragma once

#include "../manager.hpp"

#include "storage.hpp"

#include <wheels/core/assert.hpp>

namespace twist::rt::sim {

namespace user {

namespace tls {

//////////////////////////////////////////////////////////////////////

struct IVar {
  virtual ~IVar() = default;

  virtual void* SlotCtor() = 0;
  virtual void SlotDtor(void*) = 0;
};

//////////////////////////////////////////////////////////////////////

struct VarBase : IVar, ss::VarBase {
  //
};

//////////////////////////////////////////////////////////////////////

struct Key {
  size_t index;
};

//////////////////////////////////////////////////////////////////////

class Manager {
 public:
  static Manager& Instance();

  Key Register(IVar* var) {
    size_t index = AcquireSlot();
    vars_[index] = var;
    return {index};
  }

  // For local static thread_local vars
  void Visit(Key key) {
    Visit(Tls(), key.index);
  }

  Slot* Access(Key key) {
    return Access(Tls(), key.index);
  }

  void Destroy(Storage& tls) {
    for (size_t i = 0; i < count_; ++i) {
      if (tls[i].IsInit()) {
        vars_[i]->SlotDtor(tls[i].Reset());
      }
    }
  }

  void Reset() {
    count_ = 0;
  }

 private:
  size_t AcquireSlot() {
    WHEELS_VERIFY(count_ < kSlots, "Thread-local vars limit reached: " << kSlots);
    size_t index = count_++;
    return index;
  }

  static Storage& Tls();

  Slot* Access(Storage& tls, size_t index) {
    Slot* slot = &tls[index];

    if (!slot->IsInit()) {
      slot->Set(vars_[index]->SlotCtor());
    }

    return slot;
  }

  void Visit(Storage& tls, size_t index) {
    if (!tls[index].IsInit()) {
      tls[index].Set(vars_[index]->SlotCtor());
    }
  }

 private:
  IVar* vars_[kSlots];
  size_t count_{0};
};

}  // namespace tls

}  // namespace user

}  // namespace twist::rt::sim
