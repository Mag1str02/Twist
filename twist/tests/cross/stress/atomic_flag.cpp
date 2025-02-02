#include <twist/cross.hpp>

#include <twist/ed/std/atomic.hpp>

#include <twist/test/wg.hpp>
#include <twist/test/plate.hpp>

class SpinLock {
 public:
  void Lock() {
    while (locked_.test_and_set(std::memory_order::acquire)) {
      while (locked_.test(std::memory_order::relaxed)) {
        ;
      }
    }
  }

  void Unlock() {
    locked_.clear(std::memory_order::release);
  }

 private:
  twist::ed::std::atomic_flag locked_;
};

using namespace std::chrono_literals;

void TestAtomicFlag() {
  twist::cross::Run([] {
    twist::test::Plate plate;
    SpinLock spinlock;

     twist::test::WaitGroup{}
         .Add(3, [&] {
            for (size_t i = 0; i < 65'536; ++i) {
              spinlock.Lock();
              plate.Access();
              spinlock.Unlock();
            }
         })
        .Join();
  });
}

int main() {
  TestAtomicFlag();
  return 0;
}
