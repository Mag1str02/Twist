#include <wheels/test/support/terminal.hpp>

#include <unistd.h>

namespace wheels::terminal {

static std::string_view CheckTerminal(std::string_view ctrl) {
  static bool terminal = isatty(1);
  return terminal ? ctrl : "";
}

std::string_view Red() {
  return CheckTerminal("\033[0;31m");
}

std::string_view Green() {
  return CheckTerminal("\033[1;32m");
}

std::string_view Yellow() {
  return CheckTerminal("\033[1;33m");
}

std::string_view Cyan() {
  return CheckTerminal("\033[0;36m");
}

std::string_view Magenta() {
  return CheckTerminal("\033[0;35m");
}

std::string_view Reset() {
  return CheckTerminal("\033[0m");
}

}  // namespace wheels::terminal
