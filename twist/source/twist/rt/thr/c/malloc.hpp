#pragma once

#include <cstdlib>

namespace twist::rt::thr {

namespace c {

inline void* malloc(size_t size) {
  return ::malloc(size);
}

inline void free(void* ptr) {
  ::free(ptr);
}

}  // namespace c

}  // namespace twist::rt::thr
