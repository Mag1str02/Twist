#pragma once

#include "wake_key.hpp"

#include "../system/wait.hpp"

namespace futex_like {

namespace atomic {

inline void WakeOne(WakeKey key) {
  system::WakeOne(key.loc);
}

inline void WakeAll(WakeKey key) {
  system::WakeAll(key.loc);
}

}  // namespace atomic

}  // namespace futex_like
