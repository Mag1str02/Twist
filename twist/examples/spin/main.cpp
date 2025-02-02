#include <twist/cross.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/wait/spin.hpp>
#include <twist/ed/fmt/print.hpp>

#include <vector>

//////////////////////////////////////////////////////////////////////

// Naive Test-and-Set (TAS) spinlock

// Correctly implementing a spinlock in C++
// https://rigtorp.se/spinlock/

class SpinLock {
 public:
  void Lock() {
    twist::ed::SpinWait spin_wait;
    while (locked_.exchange(true)) {
      spin_wait();
    }
  }

  void Unlock() {
    locked_.store(false);
  }

 private:
  twist::ed::std::atomic<bool> locked_{false};
};

//////////////////////////////////////////////////////////////////////

int main() {
  twist::cross::Run([] {
    std::vector<twist::ed::std::thread> threads;

    SpinLock spinlock;
    size_t cs = 0;

    for (size_t i = 0; i < 4; ++i) {
      threads.emplace_back([&] {
        for (size_t j = 0; j < 100'500; ++j) {
          spinlock.Lock();
          ++cs;
          spinlock.Unlock();
        }
      });
    }

    for (auto&& t : threads) {
      t.join();
    }

    twist::ed::fmt::println("# critical sections: {}", cs);
  });

  return 0;
}
