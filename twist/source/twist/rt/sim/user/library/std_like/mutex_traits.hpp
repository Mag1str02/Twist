#pragma once

namespace twist::rt::sim {

namespace user::library::std_like {

template <typename MutexType>
struct MutexTraits {
  static constexpr bool kNative = false;
};

}  // user::library::std_like

}  // namespace twist::rt::sim
