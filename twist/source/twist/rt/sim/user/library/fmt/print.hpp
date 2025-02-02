#pragma once

#include <twist/rt/sim/user/fmt/temp.hpp>

#include <twist/rt/sim/user/syscall/write.hpp>

namespace twist::rt::sim {

namespace user::library::fmt {

template <typename... Args>
void print(int fd, ::fmt::format_string<Args...> format_str, Args&&... args) {
  syscall::Write(fd, user::fmt::FormatTemp(format_str, std::forward<Args>(args)...));
}

template <typename... Args>
void print(::fmt::format_string<Args...> format_str, Args&&... args) {
  print(1, format_str, std::forward<Args>(args)...);
}

template <typename... Args>
void println(int fd, ::fmt::format_string<Args...> format_str, Args&&... args) {
  syscall::Write(fd, user::fmt::FormatLnTemp(format_str, std::forward<Args>(args)...));
}

template <typename... Args>
void println(::fmt::format_string<Args...> format_str, Args&&... args) {
  println(1, format_str, std::forward<Args>(args)...);
}

}  // namespace user::library::fmt

}  // namespace twist::rt::sim
