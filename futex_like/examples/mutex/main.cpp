#include "mutex.hpp"

#include <mutex>
#include <thread>
#include <vector>
#include <iostream>

int main() {
  Mutex mutex;
  size_t cs = 0;

  std::vector<std::thread> contenders;

  for (size_t i = 0; i < 4; ++i) {
    contenders.emplace_back([&] {
      for (size_t j = 0; j < 1024; ++j) {
        std::lock_guard guard{mutex};
        ++cs;
      }
    });
  }

  for (auto& t : contenders) {
    t.join();
  }

  std::cout << "# critical sections = " << cs << std::endl;

  return 0;
}
