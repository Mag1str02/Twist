#include <twist/cross.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/thread.hpp>
#include <twist/ed/wait/futex.hpp>
#include <twist/ed/fmt/print.hpp>

#include <chrono>

using namespace std::chrono_literals;

//////////////////////////////////////////////////////////////////////

class OneShotEvent {
  struct States {
    enum _ : uint32_t {
      NotSet = 0,
      Set = 1
    };
  };

 public:
  void Wait() {
    if (state_.load() != States::Set) {
      twist::ed::futex::Wait(state_, /*old=*/States::NotSet);
    }
  }

  // One-shot
  void Set() {
    // NB: before store
    auto wake_key = twist::ed::futex::PrepareWake(state_);

    state_.store(States::Set);
    twist::ed::futex::WakeAll(wake_key);
  }

 private:
  twist::ed::std::atomic<uint32_t> state_{States::NotSet};
};

//////////////////////////////////////////////////////////////////////

int main() {
  twist::cross::Run([] {
    std::string data;
    OneShotEvent flag;

    twist::ed::std::thread producer([&] {
      twist::ed::std::this_thread::sleep_for(3s);

      data = "Hello";
      flag.Set();
    });

    flag.Wait();

    twist::ed::fmt::println("Produced: {}", data);

    producer.join();
  });

  return 0;
}
