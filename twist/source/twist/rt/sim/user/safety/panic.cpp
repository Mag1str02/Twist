#include "panic.hpp"

#include <twist/rt/sim/user/syscall/id.hpp>
#include <twist/rt/sim/user/syscall/write.hpp>
#include <twist/rt/sim/user/syscall/abort.hpp>

#include <twist/wheels/fmt/source_location.hpp>

#include <fmt/core.h>

namespace twist::rt::sim {

namespace user {

static const size_t kBufferSize = 1024;

static char buf[kBufferSize];

void Panic(system::Status status, std::string_view error, wheels::SourceLocation loc) {
  {
    auto id = syscall::GetId();

    // No dynamic allocations here
    auto written = fmt::format_to_n(
        buf, kBufferSize, "Thread #{} panicked at {}: {}",
        id, loc, error);

    syscall::Write(/*fd=*/2, {buf, written.size});
  }

  syscall::Abort(status);
}

}  // namespace user

}  // namespace twist::rt::sim
