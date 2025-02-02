#include "pin.hpp"

#include <wheels/core/compiler.hpp>
#include <wheels/core/panic.hpp>

#include <cstring>

#if LINUX
#include <pthread.h>
#include <sys/sysinfo.h>
#endif

namespace twist::rt {
namespace thr {

void PinThisThread(int desired_cpu) {
#if LINUX
  auto self = pthread_self();

  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(desired_cpu, &cpuset);

  int result = pthread_setaffinity_np(self, sizeof(cpu_set_t), &cpuset);
  if (result != 0) {
    WHEELS_PANIC("Error " << result
                          << " on calling pthread_setaffinity_np on core "
                          << desired_cpu << ": " << std::strerror(result));
  } else {
#ifndef NDEBUG
    /*
    SyncCout() << "Thread " << self << " pinned to core " << desired_cpu
               << std::endl;
    */
#endif
  }

#else
#pragma message("PinThisThread is not implemented on current platform")
  WHEELS_UNUSED(desired_cpu);
#endif
}

}  // namespace thr
}  // namespace twist::rt
