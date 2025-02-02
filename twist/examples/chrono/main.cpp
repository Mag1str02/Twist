#include <twist/cross.hpp>

#include <twist/ed/std/chrono.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/fmt/print.hpp>

#include <fmt/chrono.h>

using namespace std::chrono_literals;

auto SteadyNow() {
  return twist::ed::std::chrono::steady_clock::now();
}

int main() {
  twist::cross::Run([] {
    auto start_time = SteadyNow();

    // Time is compressed in simulation
    auto pause = twist::build::Sim() ? 1'000'000s : 1s;

    twist::ed::std::this_thread::sleep_for(pause);

    twist::ed::fmt::println("Elapsed: {}", SteadyNow() - start_time);
  });

  return 0;
}
