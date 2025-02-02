#include <wheels/system/quick_exit.hpp>

#include <cstdlib>

namespace wheels {

void QuickExit(int exit_code) {
#if LINUX
  std::quick_exit(exit_code);
#elif APPLE
  std::exit(exit_code);
#else
#error "QuickExit is not supported"
#endif
}


}  // namespace wheels
