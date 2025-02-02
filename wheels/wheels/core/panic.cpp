#include <wheels/core/panic.hpp>

#include <iostream>
#include <mutex>

namespace wheels {

static std::mutex mutex;

void Panic(std::string_view error, SourceLocation where) {
  {
    std::lock_guard guard(mutex);
    std::cerr << "Panicked at " << where << ": " << error << std::endl;
  }

  std::abort();
}

}  // namespace wheels
