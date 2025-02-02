#include "cond_pthread.hpp"
#include "mutex.hpp"

#include <twist/sim.hpp>

#include <twist/test/wg.hpp>

#include <fmt/core.h>

class CountDown {
 public:
  CountDown(size_t count)
      : count_(count) {
  }

  void Down() {
    auto key = twist::ed::futex::PrepareWake(count_);
    if (count_.fetch_sub(1) == 1) {
      twist::ed::futex::WakeOne(key);
    }
  }

  void Wait() {
    while (true) {
      size_t count = count_.load();
      if (count == 0) {
        break;
      } else {
        twist::ed::futex::Wait(count_, count);
      }
    }
  }

 private:
  twist::ed::std::atomic<uint32_t> count_;
};

class WorkQueue {
 public:
  bool Take() {
    mutex_.Lock();

    while ((work_ == 0) && !closed_) {
      take_.Wait(mutex_);
    }

    if (work_ > 0) {
      --work_;
      mutex_.Unlock();
      return true;
    } else {
      // closed_
      mutex_.Unlock();
      return false;
    }
  }

  void Add() {
    {
      mutex_.Lock();
      ++work_;
      mutex_.Unlock();
    }

    take_.Signal();
  }

  void Close() {
    {
      mutex_.Lock();
      closed_ = true;
      mutex_.Unlock();
    }

    take_.Broadcast();
  }

 private:
  Mutex mutex_;
  PthreadLikeCondVar take_;
  size_t work_{0};  // Guarded by mutex_
  bool closed_{false};  // Guarded by mutex_
};

void TestCase(size_t generators, size_t workers, size_t work) {
  // Shared state
  twist::ed::std::atomic<size_t> work_todo{work};
  twist::ed::std::atomic<size_t> gen{generators};
  WorkQueue work_queue;
  CountDown work_done{work};

  // Generators + workers
  twist::test::WaitGroup wg;

  wg.Add(generators, [&] {
    // Generator

    size_t w = work_todo.load();

    while (true) {
      if (w == 0) {
        if (gen.fetch_sub(1) == 1) {
          // Last generator
          work_done.Wait();
          work_queue.Close();
        }
        break;
      } else if (work_todo.compare_exchange_weak(w, w - 1)) {
        work_queue.Add();
      } else {
        // w has been re-read
      }
    }
  });

  wg.Add(workers, [&] {
    // Worker
    while (work_queue.Take()) {
      work_done.Down();
    }
  });

  wg.Join();
}

void Test() {
  TestCase(2, 3, 5);
}

static_assert(twist::build::IsolatedSim());

int main() {
  // A Randomized Scheduler with Probabilistic Guarantees of Finding Bugs
  // https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/asplos277-pct.pdf

  twist::sim::sched::PctScheduler pct{{.depth = 7, .max_runs = 256'000}};

  auto exp = twist::sim::Explore(pct, Test);

  fmt::println("Simulations: {}", exp.sims);

  if (exp.found) {
    // Schedule, simulation result
    auto [schedule, result] = *exp.found;
    twist::sim::Print(Test, schedule);
    fmt::println("Stderr: {}", result.std_err);
  } else {
    fmt::println("Seems correct");
  }

  return 0;
}
