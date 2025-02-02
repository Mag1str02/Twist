#include "buffer.hpp"

namespace twist::rt::sim {

namespace system {

namespace log {

static const size_t kBufferSize = 2048;

wheels::MutableMemView Buffer() {
  static char buffer[kBufferSize];

  return {buffer, kBufferSize};
}

wheels::MutableMemView CommentBuffer() {
  static char buffer[kBufferSize];

  return {buffer, kBufferSize};
}


}  // namespace log

}  // namespace system

}  // namespace twist::rt::sim
