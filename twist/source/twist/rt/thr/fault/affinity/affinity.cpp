#include "affinity.hpp"

#include "pin.hpp"

#include <algorithm>
#include <thread>
#include <random>
#include <cstring>
#include <iostream>
#include <cstdlib>

#if defined(__TWIST_FAULTY__) && !defined(__TWIST_SIM__)
#define PIN_THREADS 1
#endif

namespace twist::test {

#if defined(PIN_THREADS)

// Returns 1 if std::thread::hardware_concurrency failed to count cores
static int GetCpuCount() {
  return std::max(1, static_cast<int>(std::thread::hardware_concurrency()));
}

struct RandomCpuId {
  int value_;

  RandomCpuId() : value_(ChooseCore()) {
  }

  static int ChooseCore() {
    int value = TryJenkinsExecutionNumber();
    if ((value == -1) || (value >= GetCpuCount())) {
      value = ChooseRandomCore();
    }
    return value;
  }

  static int TryJenkinsExecutionNumber() {
    int result = -1;
    if (const char* env_p = std::getenv("EXECUTOR_NUMBER")) {
      try {
        result = std::stoi(env_p);
      } catch (const std::exception& ia) {
#ifdef DEBUG
        std::cerr << "Failed parsing 'EXECUTOR_NUMBER=" << env_p
                  << "' from environment: " << ia.what() << "\n";
#endif
      }
    }
    return result;
  }

  static int ChooseRandomCore() {
    std::random_device random;
    std::default_random_engine entropy(random());
    std::uniform_int_distribution<int> distribution(0, GetCpuCount() - 1);
    return distribution(entropy);
  }
};

static void PinToSingleCore() {
  static RandomCpuId desired_cpu;
  twist::rt::thr::PinThisThread(desired_cpu.value_);
}

#endif

void SetThreadAffinity() {
#if defined(PIN_THREADS)
  PinToSingleCore();
#endif
}

}  // namespace twist::test
