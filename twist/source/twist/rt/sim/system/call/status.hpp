#pragma once

namespace twist::rt::sim {

namespace system::call {

enum Status : int {
  Ok = 0,
  Timeout = 1,
  Interrupted = 2,  // Spurious wake-up
};

}  // namespace system::call

}  // namespace twist::rt::sim
