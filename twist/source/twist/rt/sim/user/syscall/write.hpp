#pragma once

#include <string_view>

namespace twist::rt::sim {

namespace user::syscall {

void Write(int fd, std::string_view buf);

}  // namespace user::syscall

}  // namespace twist::rt::sim
