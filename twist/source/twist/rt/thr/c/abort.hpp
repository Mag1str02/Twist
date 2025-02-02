#pragma once

#include <cstdlib>

namespace twist::rt::thr {

[[noreturn]] inline void abort() {
  ::abort();
}

}  // namespace twist::rt::thr
