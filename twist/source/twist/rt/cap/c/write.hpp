#pragma once

#if defined(__TWIST_SIM__)

#include <twist/rt/sim/user/library/system/write.hpp>

namespace twist::rt::cap {

namespace c {

inline void write(int fd, const char* buf, size_t size) {
  sim::user::library::system::Write(fd, {buf, size});
}

}  // namespace c

}  // namespace twist::rt::cap

#else

#include <twist/rt/thr/system/write.hpp>

namespace twist::rt::cap {

namespace c {

inline void write(int fd, const char* buf, size_t size) {
  thread::Write(fd, {buf, size});
}

}  // namespace c

}  // namespace twist::rt::cap

#endif
