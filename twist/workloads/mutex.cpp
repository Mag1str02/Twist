#include <twist/mod/cross.hpp>

#include <twist/ed/std/mutex.hpp>
#include <twist/ed/fmt/print.hpp>

#include <twist/test/wg.hpp>
#include <twist/test/plate.hpp>

using namespace std::chrono_literals;

int main() {
  twist::cross::Run([] {
    twist::test::WaitGroup wg;
    twist::test::Plate plate;

    twist::ed::std::mutex mutex_;

    wg.Add(7, [&] {
      for (size_t i = 0; i < 16384; ++i) {
        {
          std::lock_guard guard(mutex_);

          plate.Access();
        }
      }
    });

    wg.Join();

    twist::ed::fmt::println("Cs = {}", plate.AccessCount());
  });

  return 0;
}
