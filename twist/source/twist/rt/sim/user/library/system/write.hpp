#pragma once

#include <twist/rt/sim/user/syscall/write.hpp>

namespace twist::rt::sim {

namespace user::library::system {

inline void Write(int fd, std::string_view buf) {
  syscall::Write(fd, buf);
}

}  // namespace user::library::system

}  // namespace twist::rt::sim
