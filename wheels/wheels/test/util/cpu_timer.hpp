#pragma once

#include <ctime>
#include <cstdlib>
#include <chrono>
#include <cassert>

namespace wheels {

class ProcessCPUTimer {
 public:
  ProcessCPUTimer() {
    Reset();
  }

  std::chrono::microseconds Spent() const {
    return std::chrono::microseconds(SpentMicros());
  }

  // Backward compatibility
  std::chrono::microseconds Elapsed() const {
    return Spent();
  }

  void Reset() {
    start_ts_ = GetTimeInSeconds();
  }

 private:
  double GetTimeInSeconds() const;

 private:
  size_t SpentMicros() const {
    const double clocks = GetTimeInSeconds() - start_ts_;
    return clocks * 1'000'000;
  }

 private:
  double start_ts_;
};

}  // namespace wheels

namespace wheels {

class ThreadCPUTimer {
 public:
  ThreadCPUTimer() {
    Reset();
  }

  std::chrono::nanoseconds Spent() const {
    return std::chrono::nanoseconds(SpentNanos());
  }

  // Backward compatibility
  std::chrono::nanoseconds Elapsed() const {
    return Spent();
  }

  void Reset() {
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start_);
  }

 private:
  uint64_t SpentNanos() const {
    struct timespec now;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now);

    return ToNanos(now) - ToNanos(start_);
  }

  static uint64_t ToNanos(const struct timespec& tp) {
    return tp.tv_sec * 1'000'000'000 + tp.tv_nsec;
  }

 private:
  struct timespec start_;
};

}  // namespace wheels
