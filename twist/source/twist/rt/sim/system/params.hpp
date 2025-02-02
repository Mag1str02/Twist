#pragma once

#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <optional>

namespace twist::rt::sim {

namespace system {

struct Params {
  std::chrono::nanoseconds tick = std::chrono::microseconds(10);
  bool allow_detached_threads = true;  // User memory isolation mode only

  std::optional<size_t> min_stack_size;
  bool check_stack_pointer = true;
  std::optional<unsigned char> memset_stacks;  // User memory isolation mode only

  bool zero_memory = false;
  bool randomize_malloc = false;
  std::optional<unsigned char> memset_malloc;  // User memory isolation mode only
  bool malloc_happens_before = true;

  size_t spin_wait_yield_threshold = 2;
  size_t hardware_concurrency = 4;

  bool crash_on_abort = true;

  bool forward_stdout = true;

  // User memory isolation mode only:
  bool digest_atomic_loads = false;
  // Digest atomic loads if possible
  bool allow_digest_atomic_loads = true;

  bool verbose = false;
};

}  // namespace system

}  // namespace twist::rt::sim
