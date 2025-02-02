#pragma once

#include <string_view>

namespace wheels::terminal {

std::string_view Red();
std::string_view Green();
std::string_view Yellow();
std::string_view Cyan();
std::string_view Magenta();
std::string_view Reset();

}  // namespace wheels::terminal

#define GREEN(output) \
  ::wheels::terminal::Green() << output << ::wheels::terminal::Reset()

#define RED(output) \
  ::wheels::terminal::Red() << output << ::wheels::terminal::Reset()

#define CYAN(output) \
  ::wheels::terminal::Cyan() << output << ::wheels::terminal::Reset()

#define MAGENTA(output) \
  ::wheels::terminal::Magenta() << output << ::wheels::terminal::Reset()

#define YELLOW(output) \
  ::wheels::terminal::Yellow() << output << ::wheels::terminal::Reset()
