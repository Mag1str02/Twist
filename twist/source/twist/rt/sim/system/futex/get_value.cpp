#include "get_value.hpp"

namespace twist::rt::sim {

namespace system {

static uint64_t GetLowHalf(uint64_t v) {
  return v & 0xFFFFFFFF;
}

static uint64_t GetHighHalf(uint64_t v) {
  return (v >> 32) & 0xFFFFFFFF;
}

uint64_t GetFutexValue(sync::AtomicVar* atomic, uint64_t atomic_value,
                              FutexLoc loc) {
  if (loc.type == FutexAtomicType::Uint32) {
    WHEELS_VERIFY(atomic->size == loc.AtomicSize(), "Atomic size mismatch");
    return atomic_value;
  } else if (loc.type == FutexAtomicType::Uint64Half) {
    // futex::Ref{Low,High}Half

    WHEELS_VERIFY(atomic->size == 8, "Unsupported atomic size");
    WHEELS_VERIFY(atomic->size == loc.AtomicSize(), "Atomic size mismatch");

    if (loc.part == FutexAtomicPart::LowHalf) {
      return GetLowHalf(atomic_value);
    } else if (loc.part == FutexAtomicPart::HighHalf) {
      return GetHighHalf(atomic_value);
    } else {
      WHEELS_PANIC("Unexpected futex location");
    }

  } else if (loc.type == FutexAtomicType::Std) {
    // atomic<T>::wait
    return atomic_value;
  } else {
    WHEELS_PANIC("Futex location not supported");
  }
}

}  // namespace system

}  // namespace twist::rt::sim