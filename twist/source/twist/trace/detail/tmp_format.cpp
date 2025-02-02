#include "tmp_format.hpp"

namespace twist::trace {

namespace detail {

static const size_t kBufferSize = 1024;

wheels::MutableMemView StaticFmtBuffer() {
#if defined(__TWIST_SIM__)
  static char buffer[kBufferSize];
#else
  static thread_local char buffer[kBufferSize];
#endif
  return {buffer, kBufferSize};
}

}  // namespace detail

}  // namespace twist::trace
