#pragma once

#include <cstdint>

#include <array>
#include <utility>

namespace twist::rt::sim {

namespace user {

namespace tls {

//////////////////////////////////////////////////////////////////////

extern const uintptr_t kSlotUninitialized;

class Slot {
 public:
  void Set(void* data) {
    ptr_ = reinterpret_cast<uintptr_t>(data);
  }

  template <typename T>
  T* GetTyped() const {
    return reinterpret_cast<T*>(ptr_);
  }

  bool IsInit() const {
    return ptr_ != kSlotUninitialized;
  }

  void* Reset() {
    uintptr_t ptr = std::exchange(ptr_, kSlotUninitialized);
    return reinterpret_cast<void*>(ptr);
  }

 private:
  uintptr_t ptr_ = kSlotUninitialized;
};

//////////////////////////////////////////////////////////////////////

static inline const size_t kSlots = 128;

using Storage = std::array<Slot, kSlots>;

}  // namespace tls

}  // namespace user

}  // namespace twist::rt::sim
