#include "malloc.hpp"

#if defined(__TWIST_SIM_ISOLATION__)

#include <twist/rt/sim/system/simulator.hpp>

namespace twist::rt::sim {

namespace user::library::c {

void* malloc(size_t size) {
  return system::Simulator::Current()->UserMalloc(size);
}

void free(void* ptr) {
  system::Simulator::Current()->UserFree(ptr);
}

}  // namespace user::library::c

}  // namespace twist::rt::sim

#else

namespace twist::rt::sim {

namespace user::library::mem {

void* malloc(size_t size) {
  return std::malloc(size);
}

void free(void* ptr) {
  return std::free(ptr);
}

}  // namespace user::library::mem

}  // namespace twist::rt::sim

#endif