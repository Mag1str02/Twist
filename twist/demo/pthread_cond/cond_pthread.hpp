#pragma once

#include "mutex.hpp"

#include <twist/ed/std/atomic.hpp>
#include <twist/ed/std/mutex.hpp>

/*
 * Simplified version of pthread condition variable
 *
 * https://sourceware.org/git/?p=glibc.git;a=blob;f=nptl/pthread_cond_wait.c;h=806c432d13497c912fbacec0b2591fe92be42f07;hb=HEAD
 *
 * - No memory orders
 * - No expiration time / timeout
 * - No spinning
 * - Internal mutex separated from wrefs_
 */

class PthreadLikeCondVar {
 public:
  PthreadLikeCondVar()
      : signal_mutex_() {
  }

  void Wait(Mutex& user) {
    // Doorway

    uint64_t wseq = wseq_.fetch_add(2);

    // Group index
    uint32_t g = wseq & 1;
    // Sequence number
    uint64_t seq = wseq >> 1;

    wrefs_.fetch_add(1);

    user.Unlock();

    {
      // Wait section

      {
        // Acquire signal

        uint32_t signals = g_signals_[g].load();

        do {
          while (true) {
            {
              // Spin
            }

            if ((signals & 1) != 0) {
              goto done;
            }

            if ((signals >> 1) > 0) {
              break;
            }

            {
              // Futex

              g_refs_[g].fetch_add(2);

              if (((g_signals_[g].load() & 1) != 0) ||
                  (seq < (g1_start_.load() >> 1))) {
                // Group is closed
                DecrementGRefs(g);
                goto done;
              }

              // Wait
              twist::ed::futex::Wait(g_signals_[g], 0);

              DecrementGRefs(g);
            }

            signals = g_signals_[g].load();
          }
        } while (!g_signals_[g].compare_exchange_weak(signals, signals - 2));
      }

      {
        uint64_t g1_start = g1_start_.load();

        if (seq < (g1_start >> 1)) {
          if (((g1_start & 1) ^ 1) == g) {
            // Potential stealing

            uint32_t s = g_signals_[g].load();

            auto g_wake_key = twist::ed::futex::PrepareWake(g_signals_[g]);

            while (g1_start_.load() == g1_start) {
              if (((s & 1) != 0) ||
                  g_signals_[g].compare_exchange_weak(s, s + 2)) {
                twist::ed::futex::WakeOne(g_wake_key);
                break;
              }
            }

          }
        }
      }

    }

    done:

    wrefs_.fetch_sub(1);

    user.Lock();
  }

  void Signal() {
    if (wrefs_.load() == 0) {
      // No waiters
      return;
    }

    signal_mutex_.lock();

    uint64_t wseq = wseq_.load();
    uint32_t g1 = (wseq & 1) ^ 1;
    wseq >>= 1;

    bool do_futex_wake = false;

    if ((g_size_[g1] > 0) || QuiesceAndSwitchG1(wseq, g1)) {
      // Add signal
      g_signals_[g1].fetch_add(2);
      // Decrease group size
      g_size_[g1]--;
      // Wake waiter
      do_futex_wake = true;
    }

    auto g1_wake_key = twist::ed::futex::PrepareWake(g_signals_[g1]);

    signal_mutex_.unlock();

    if (do_futex_wake) {
      twist::ed::futex::WakeOne(g1_wake_key);
    }
  }

  void Broadcast() {
    if (wrefs_.load() == 0) {
      // No waiters
      return;
    }

    signal_mutex_.lock();

    uint64_t wseq = wseq_.load();
    uint32_t g2 = wseq & 1;
    uint32_t g1 = g2 ^ 1;

    wseq >>= 1;

    bool do_futex_wake = false;

    {
      // G1
      if (g_size_[g1] > 0) {
        auto g1_signals_key = twist::ed::futex::PrepareWake(g_signals_[g1]);

        g_signals_[g1].fetch_add(g_size_[g1] << 1);
        g_size_[g1] = 0;

        // We need to wake G1 waiters before we quiesce G1 below. - Why?
        twist::ed::futex::WakeAll(g1_signals_key);
      }
    }

    {
      // G2

      // NB: g1 may change
      if (QuiesceAndSwitchG1(wseq, g1)) {
        g_signals_[g1].fetch_add(g_size_[g1] << 1);
        g_size_[g1] = 0;

        do_futex_wake = true;
      }
    }


    signal_mutex_.unlock();

    if (do_futex_wake) {
      auto g1_signals_key = twist::ed::futex::PrepareWake(g_signals_[g1]);
      twist::ed::futex::WakeAll(g1_signals_key);
    }
  }


  ~PthreadLikeCondVar() {
    //
  }

 private:
  void DecrementGRefs(uint32_t g) {
    if (g_refs_[g].fetch_sub(2) == 3) {
      // Last one + switch-wait is set

      auto wake_key = twist::ed::futex::PrepareWake(g_refs_[g]);

      // Reset switch-wait flag
      g_refs_[g].fetch_and(~(uint32_t)1);
      twist::ed::futex::WakeAll(wake_key);
    }
  }

  // Holding signaler_mutex_
  bool QuiesceAndSwitchG1(uint64_t wseq, uint32_t& g1) {
    // 0)
    uint64_t old_g1_orig_size = g1_orig_size_;
    uint64_t old_g1_start = g1_start_.load() >> 1;

    if ((wseq - (old_g1_start + old_g1_orig_size)) == 0) {
      return false;  // Empty G1
    }

    // 1) Mark current G1 as closed
    g_signals_[g1].fetch_or(1);

    // 2) Pass g_signals_[g1] futex waiters

    uint32_t r = g_refs_[g1].fetch_or(0);  // load?

    while ((r >> 1) > 0) {
      {
        // Spin
      }

      if ((r >> 1) > 0) {
        // Park

        // Set 'wake signaler' flag
        r = g_refs_[g1].fetch_or(1);

        if ((r >> 1) > 0) {
          // Wait for last futexed waiter
          twist::ed::futex::Wait(g_refs_[g1], r);
        }

        // Reload
        r = g_refs_[g1].load();
      }
    }

    // 3) Update g1_start
    {
      // NB: switch G2 index
      {
        g1_start_.fetch_add((old_g1_orig_size << 1) + (g1 == 1 ? 1 : -1));
      }
    }

    // 4) Reopen g1
    g_signals_[g1] = 0;

    // 5)
    wseq = wseq_.fetch_xor(1) >> 1;

    g1 ^= 1;  // NB: passed by reference

    // New group
    uint64_t orig_size = wseq - (old_g1_start + old_g1_orig_size);
    g1_orig_size_ = orig_size;
    g_size_[g1] += orig_size;  // TODO: = is enough?

    // No cancellation

    return true;  // new G1
  }

 private:
  // Waiter sequence counter
  twist::ed::std::atomic<uint64_t> wseq_{0};

  // Starting position of G1 (inclusive)
  twist::ed::std::atomic<uint64_t> g1_start_{0};

  // Waiter counter
  twist::ed::std::atomic<uint32_t> wrefs_{0};

  // For each of the two groups, we have:

  // The number of signals that can still be consumed
  twist::ed::std::atomic<uint32_t> g_signals_[2] = {0, 0};

  // Futex waiter reference count
  twist::ed::std::atomic<uint32_t> g_refs_[2] = {0, 0};

  twist::ed::std::mutex signal_mutex_;

  // Waiters remaining in this group (i.e., which have not been signaled yet
  // Guarded by signal_mutex_
  uint32_t g_size_[2] = {0, 0};

  // Initial size of G1
  // Guarded by signal_mutex_
  uint64_t g1_orig_size_{0};
};
