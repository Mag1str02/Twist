#include <futex_like/wait_wake.hpp>

#include <atomic>
#include <thread>
#include <iostream>

int main() {
  int data;
  std::atomic_uint32_t flag{0};

  std::thread producer([&] {
    data = 42;

    auto waiters = futex_like::PrepareWake(flag);
    flag.store(1);
    futex_like::WakeOne(waiters);
  });

  futex_like::Wait(flag, 0);
  std::cout << data << std::endl;  // Prints 42

  producer.join();

  return 0;
}
