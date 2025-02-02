#pragma once

#if __has_feature(address_sanitizer)

#include <sanitizer/lsan_interface.h>

namespace wheels {

// Mark intentional memory leak
// The heap object referenced by pointer 'ptr' will be annotated as a leak.

inline void MarkLeakingObjectPtr(void* ptr) {
  __lsan_ignore_object(ptr);
}

using LeaksScope = __lsan::ScopedDisabler;

}  // namespace wheels

#else

namespace wheels {

inline void MarkLeakingObjectPtr(void*) {
}

struct LeaksScope {
  LeaksScope() {
  }
  ~LeaksScope() {
  }
};

}  // namespace wheels

#endif
