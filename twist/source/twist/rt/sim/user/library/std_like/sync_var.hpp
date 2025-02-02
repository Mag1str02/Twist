#pragma once

#include "atomic_base_int.hpp"

#include <twist/rt/sim/user/syscall/futex.hpp>

#include <wheels/core/compiler.hpp>
#include <wheels/core/source_location.hpp>

// std::memory_order
#include <atomic>
// std::memcmp
#include <cstring>
#include <type_traits>
// std::exchange
#include <utility>
#include <cstdint>

namespace twist::rt::sim {

namespace user::library::std_like {

class SyncVar : protected AtomicBase<uint32_t> {
 public:
  using Value = uint32_t;  // NOLINT

  using Base = AtomicBase<uint32_t>;

  SyncVar(wheels::SourceLocation source_loc)
      : Base(source_loc) {
  }

  // Non-copyable
  SyncVar(const SyncVar&) = delete;
  SyncVar& operator=(const SyncVar&) = delete;

  // Non-movable
  SyncVar(SyncVar&&) = delete;
  SyncVar& operator=(SyncVar&&) = delete;

  void Init(Value value, wheels::SourceLocation source_loc) {
    Base::Init(value, source_loc);
  }

  void Destroy() {
    Base::Destroy();
  }

  Value Load(std::memory_order mo, wheels::SourceLocation source_loc = wheels::SourceLocation::Current()) const noexcept {
    return Base::load(mo, source_loc);
  }

  void Store(Value new_value, std::memory_order mo, wheels::SourceLocation source_loc = wheels::SourceLocation::Current()) noexcept {
    Base::store(new_value, mo, source_loc);
  }

  Value Exchange(Value new_value, std::memory_order mo, wheels::SourceLocation source_loc = wheels::SourceLocation::Current()) noexcept {
    return Base::exchange(new_value, mo, source_loc);
  }

  Value FetchAdd(uint32_t delta, std::memory_order mo, wheels::SourceLocation source_loc = wheels::SourceLocation::Current()) noexcept {
    return Base::fetch_add(delta, mo, source_loc);
  }

  Value FetchSub(uint32_t delta, std::memory_order mo, wheels::SourceLocation source_loc = wheels::SourceLocation::Current()) noexcept {
    return Base::fetch_sub(delta, mo, source_loc);
  }

  bool CompareExchangeWeak(uint32_t& expected, uint32_t desired, std::memory_order success, std::memory_order failure, wheels::SourceLocation source_loc = wheels::SourceLocation::Current()) noexcept {
    return Base::compare_exchange_weak(expected, desired, success, failure, source_loc);
  }

  bool CompareExchangeStrong(uint32_t& expected, uint32_t desired, std::memory_order success, std::memory_order failure, wheels::SourceLocation source_loc = wheels::SourceLocation::Current()) noexcept {
    return Base::compare_exchange_strong(expected, desired, success, failure, source_loc);
  }

  system::FutexLoc FutexLoc() {
    return system::FutexLoc::AtomicUint32(this);
  }
};

}  // namespace user::library::std_like

}  // namespace twist::rt::sim
