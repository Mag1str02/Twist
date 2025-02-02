# Twist 🧵

> _He must look to meet whatever events his own fate and the stern Klothes twisted into his thread of destiny when he entered the world and his mother bore him._
>
> – Homer 🏛, Odyssey 7.193 (trans. Walter Shewring)

Systematic concurrency testing for modern C++

Keywords: [_fault injection_](https://en.wikipedia.org/wiki/Fault_injection), [_deterministic simulation_](https://www.youtube.com/watch?v=4fFDFbi3toc), [_stateless model checking_](https://users.soe.ucsc.edu/~cormac/papers/popl05.pdf)

## Documentation

[Guide (ru)](docs/ru/README.md)

## Simulation

### Demo
- (_entry_) [mutex_lock_order](demo/mutex_lock_order) – simple deadlock with 2 threads and 2 mutexes, exhaustive search with _DFS_ scheduler
- (_medium_) [bounded_queue](demo/bounded_queue) – bounded blocking queue, _Random_ scheduler
- (_expert_) [pthread_cond](demo/pthread_cond) – missed wake-up in `pthread_cond_wait`, _PCT_ scheduler
- and [more...](demo)

### Features

- Determinism
  - Scheduling
  - Randomness
  - Time
  - Memory (in isolation mode)
    - stable addresses across runs for
      - `new` allocations,
      - thread stacks,
      - static [thread-local] variables
    - content of dynamic allocations and thread stacks
- Customizable schedulers
  - _Coop_ – cooperative scheduler for manual simulation    
  - _Random_ – randomized fair scheduler
      - randomized run queue
      - randomized wait queues in `futex` / synchronization primitives
      - spurious wake-ups and `atomic::compare_exchange_weak` / `mutex::try_lock` failures
  - _PCT_ – [A Randomized Scheduler with Probabilistic Guarantees of Finding Bugs](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/02/asplos277-pct.pdf)
  - _DFS_ – exhaustive depth-first search
      - TLA<sup>+</sup>-like branching `twist::assist::Choice` and `bool twist::test::Either()`
      - `max_preemptions` bound
      - `max_steps` bound
      - non-terminating tests support (`while (true)`)
  - _Replay_
- Checks
  - Assertions (`TWIST_ASSERT`)
  - Synchronization
      - Global deadlock detection
      - [Happens-before](https://eel.is/c++draft/intro.races#11) tracking, precise data race detection (with `twist::assist::Shared<T>` annotations)
        - `std::atomic_thread_fence` support
        - [`malloc`](https://eel.is/c++draft/new.delete.dataraces) support
  - Memory
    - Leaks (automatic),
    - Double-free (automatic),
    - Heap-use-after-free (with `twist::assist::MemoryAccess` or `twist::assist::Ptr<T>`)
  - Determinism
  - Compatibility with [UndefinedBehaviourSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)
- Simulator
  - Drop-in replacements for `std` synchronization primitives
  - `futex`
  - Spurious wake-ups for futex and `std::condition_variable`, spurious failures for `std::mutex::try_lock` and `std::atomic<T>::compare_exchange_weak`  
  - Static {global, local} [thread-local] variables and class members
  - `std::this_thread::sleep_{for, until}` and `std::condition_variable::wait_{for, until}` with time compression
  - `std::thread::detach` support for background threads
  - User context switching support

## How to use

- `#include <atomic>` → `#include <twist/ed/std/atomic.hpp>`
- `std::atomic<T>` → `twist::ed::std::atomic<T>`

### Twist-ed `SpinLock`

```cpp
#include <twist/ed/std/atomic.hpp>
#include <twist/ed/wait/spin.hpp>

class SpinLock {
 public:
  void Lock() {
    // Execution backend-aware backoff for busy waiting
    twist::ed::SpinWait spin_wait;
    
    while (locked_.exchange(true)) {  // <- Faults injected here
      spin_wait();  // ~ spin_wait.Spin();
    }
  }

  void Unlock() {
    locked_.store(false);  // <- Faults injected here
  }

 private:
  // Drop-in replacement for std::atomic<T>
  twist::ed::std::atomic<bool> locked_{false};
};
```

### Examples

- [futex](/examples/futex/main.cpp) – `twist::ed::futex::Wait`
- [spin](/examples/spin/main.cpp) – `twist::ed::SpinWait`
- [chrono](/examples/chrono/main.cpp) – `twist::ed::std::this_thread::sleep_for` / time compression
- [static](/examples/static/main.cpp) – static {global, local} variables, static class members  
- [thread_local](/examples/thread_local/main.cpp) – static thread-local variables
- [assist](/examples/assist/main.cpp) – `twist::assist::`
- [trace](/examples/trace/main.cpp) – `twist::trace::`

### Standard library support

[`twist/ed/std/`](/source/twist/ed/std), namespace `twist::ed::std::`
- `atomic<T>`, `atomic_flag` (`atomic.hpp`)
- `mutex`, `timed_mutex` (`mutex.hpp`)
- `shared_mutex` (`shared_mutex.hpp`)  
- `condition_variable` (`condition_variable.hpp`)
- `thread` (`thread.hpp`)
- `this_thread::` (`thread.hpp`)
  - `yield`
  - `sleep_for`, `sleep_until`
  - `get_id`
- `random_device` (`random.hpp`)
- `chrono::` (`chrono.hpp`)
  - `system_clock`  
  - `steady_clock`
  - `high_resolution_clock`

## Installation

### FetchContent

```cmake
# https://cmake.org/cmake/help/latest/module/FetchContent.html
include(FetchContent)

FetchContent_Declare(
        twist
        GIT_REPOSITORY https://gitlab.com/Lipovsky/twist.git
        GIT_TAG master
)
FetchContent_MakeAvailable(twist)

add_executable(my_test main.cpp)
target_link_libraries(my_test PRIVATE twist)
```
  
## CMake Options

- `TWIST_ATOMIC_WAIT` – Support `{atomic, atomic_flag}::wait` (`ON` / `OFF`)

### Runtime

- `TWIST_FAULTY` – Fault injection (`ON` / `OFF`)
  - `TWIST_FAULT_PLACEMENT` – Where to inject faults: `BEFORE` (default) sync operation / `AFTER` / `BOTH` sides
- `TWIST_SIM` – Deterministic simulation (`ON` / `OFF`)
  - `TWIST_SIM_ISOLATION` – User memory isolation (`ON` / `OFF`)
  - `TWIST_SIM_DEBUG` – Debug information for pretty stack traces (`ON` – `binutils-dev` package required / `OFF`) 

