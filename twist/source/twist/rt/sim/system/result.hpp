#pragma once

#include "status.hpp"
#include "failure.hpp"

#include <cstdlib>
#include <cstdint>
#include <optional>
#include <string>

namespace twist::rt::sim {

namespace system {

struct Result {
  Status status;
  std::optional<FailureContext> failure_context;

  std::string std_out;
  std::string std_err;

  size_t threads;
  size_t iters;

  uint64_t digest;

  bool Ok() const {
    return status == Status::Ok;
  }

  bool Complete() const {
    return status != Status::Pruned;
  }

  bool Failure() const {
    return !Ok() && Complete();
  }
};

}  // namespace system

}  // namespace twist::rt::sim
