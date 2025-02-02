#pragma once

#include <cstdint>

namespace twist::rt::thr {

namespace cores {

////////////////////////////////////////////////////////////////////////////////

// Relax in favour of the CPU owning the lock

// https://c9x.me/x86/html/file_module_x86_id_232.html
// https://aloiskraus.wordpress.com/2018/06/16/why-skylakex-cpus-are-sometimes-50-slower-how-intel-has-broken-existing-code/

inline void CpuRelax() {
#if defined(__TWIST_ARCH_x86_64)
  asm volatile("pause\n" : : : "memory");
#elif defined(__TWIST_ARCH_armv8_a_64)
  asm volatile("yield\n" : : : "memory");
#else
  ;
#endif
}

}  // namespace cores

}  // namespace twist::rt::thr
