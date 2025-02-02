#include <wheels/system/quick_exit.hpp>

#include <cstdlib>

namespace wheels {

void QuickExit(int exit_code) {
#if LINUX
  std::quick_exit(exit_code);
#else
  std::exit(exit_code);
#endif
}


}  // namespace wheels
