#pragma once

#include <cstdlib>

namespace twist::rt::thr {

[[noreturn]] inline void Exit0() {
  std::exit(0);
}

}  // namespace twist::rt::thr
