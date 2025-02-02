#pragma once

#include <chrono>

using namespace std::chrono_literals;

namespace wheels::test {

////////////////////////////////////////////////////////////////////////////////

struct GlobalOptions {
  bool forks{true};
  bool disable_time_limits{false};
};

////////////////////////////////////////////////////////////////////////////////

struct TestOptions {
  TestOptions() : time_limit(10s) {
  }

  TestOptions& TimeLimit(std::chrono::milliseconds value) {
    time_limit = value;
    return *this;
  }

  TestOptions& AdaptTLToSanitizer() {
    adapt_time_limit_to_sanitizer_ = true;
    return *this;
  }

  TestOptions& ForceFork() {
    force_fork_ = true;
    return *this;
  }

  std::chrono::milliseconds time_limit;
  bool adapt_time_limit_to_sanitizer_{false};
  bool force_fork_{false};
};

}  // namespace wheels::test
