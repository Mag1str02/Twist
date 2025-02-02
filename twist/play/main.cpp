#include <twist/cross.hpp>

#include <twist/ed/std/atomic.hpp>

#include <twist/ed/fmt/print.hpp>

int main() {
  twist::cross::Run([] {
    // Your code goes here
    twist::ed::std::atomic<bool> flag{false};
    flag.store(true);

    twist::ed::fmt::println("Hello");
  });

  return 0;
}
