#include <twist/sim.hpp>

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/thread.hpp>

#include <wheels/core/compiler.hpp>

#include <fmt/core.h>

#include <catch2/catch_test_macros.hpp>

#include <vector>

static_assert(twist::build::IsolatedSim());

TEST_CASE("FairScheduler") {
  SECTION("TicketLock") {
    // TicketLock

    class TicketLock {
      using Ticket = uint64_t;
     public:
      void Lock() {
        Ticket my = next_++;
        while (my != owner_) {
          // Repeat
        }
      }

      void Unlock() {
        owner_ = owner_ + 1;
      }

     private:
      twist::ed::std::atomic<Ticket> next_{0};
      twist::ed::std::atomic<Ticket> owner_{0};
    };

    twist::sim::sched::FairScheduler scheduler{{.time_slice = 5}};
    twist::sim::Simulator sim{&scheduler};

    auto result = sim.Run([] {
      TicketLock ticket_lock;

      std::vector<twist::ed::std::thread> contenders;

      for (size_t i = 0; i < 4; ++i) {
        contenders.emplace_back([&ticket_lock] {
          for (size_t j = 0; j < 128; ++j) {
            ticket_lock.Lock();
            ticket_lock.Unlock();
          }
        });
      }

      for (auto& t : contenders) {
        t.join();
      }
    });

    REQUIRE(result.Ok());

    fmt::println("Fair: iterations = {}", result.iters);
  }
}
