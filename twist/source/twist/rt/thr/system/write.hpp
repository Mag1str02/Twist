#pragma once

#include <unistd.h>

#include <string_view>

namespace twist::rt::thr {

inline void Write(int fd, std::string_view buf) {
  ::write(fd, buf.data(), buf.size());
}

}  // namespace twist::rt::thr
