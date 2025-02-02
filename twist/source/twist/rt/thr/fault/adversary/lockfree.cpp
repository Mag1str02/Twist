#include <twist/rt/thr/fault/adversary/lockfree.hpp>

#include <twist/rt/thr/fault/random/helpers.hpp>
#include <twist/rt/thr/fault/adversary/yielder.hpp>

#include <wheels/logging/logging.hpp>

#include <wheels/intrusive/list.hpp>
#include <wheels/core/compiler.hpp>

#include <mutex>
#include <condition_variable>

#include <set>

// TODO(Lipovsky): Separate implementation for simulation

namespace twist::rt::thr {
namespace fault {

/////////////////////////////////////////////////////////////////////

class OneShotEvent {
 public:
  void Set() {
    std::lock_guard guard(mutex_);
    ready_ = true;
    ready_cv_.notify_one();
  }

  void Await() {
    std::unique_lock lock(mutex_);
    while (!ready_) {
      ready_cv_.wait(lock);
    }
  }

 private:
  ::std::mutex mutex_;
  ::std::condition_variable ready_cv_;
  bool ready_{false};
};

// Not thread safe
class ThreadWaitQueue {
 public:
  struct WaitNode : public wheels::IntrusiveListNode<WaitNode>,
                    public OneShotEvent {
    void Wake() {
      Set();
    }
  };

  using WaitQueue = wheels::IntrusiveList<WaitNode>;

 public:
  // Suspend current thread
  void Add(WaitNode* this_thread_node) {
    wait_queue_.PushBack(this_thread_node);
  }

  void ResumeRandomThread() {
    if (!wait_queue_.IsEmpty()) {
      auto* wait_node = UnlinkRandomItem(wait_queue_);
      wait_node->Wake();
    }
  }

 private:
  WaitQueue wait_queue_;
};

/////////////////////////////////////////////////////////////////////

class ThreadInfo {
 public:
  void EnablePark() {
    ++park_;
  }

  void DisablePark() {
    --park_;
  }

  bool IsParkEnabled() const {
    return park_ > 0;
  }

 private:
  int park_{0};
};

static thread_local ThreadInfo this_thread_;  // NOLINT

class LockFreeAdversary : public IAdversary {
 public:
  LockFreeAdversary(size_t suspend_freq, size_t suspended_thread_limit,
                    size_t resume_freq, size_t yield_freq)
      : suspended_thread_limit_(suspended_thread_limit),
        suspend_(suspend_freq),
        resume_(resume_freq),
        yielder_(yield_freq) {
  }

  void Reset() override {
    suspend_.Reset();
    resume_.Reset();
    yielder_.Reset();
  }

  void Iter(size_t /*index*/) override {
  }

  void PrintReport() override {
    WHEELS_VERIFY(suspended_thread_count_ == 0, "some threads still suspended");
  }

  // Per-thread methods

  void Enter() override {
    // TODO(Lipovsky): check thread ids
    ++thread_count_;
  }

  void EnablePark() override {
    this_thread_.EnablePark();
  }

  void DisablePark() override {
    this_thread_.DisablePark();
  }

  void Fault(FaultPlacement) override {
    size_t inject_count = fault_count_.fetch_add(1, std::memory_order::relaxed);

    if (suspend_.Test()) {
      SuspendThisThread(inject_count);
    } else {
      yielder_.MaybeYield();
    }
  }

  void ReportProgress() override {
    // Wait-free on fast path, locking on slow path
    if (suspended_thread_count_ > 0 && resume_.Test()) {
      ResumeThread();
    }
  }

  void Exit() override {
    size_t threads_left = thread_count_--;

    WHEELS_UNUSED(threads_left);
    // LOG_SIMPLE("Thread exit (left: " << threads_left << ")");

    // At least one running thread!
    ResumeThread();
  }

 private:
  void ResumeThread() {
    std::lock_guard guard(mutex_);
    suspended_threads_.ResumeRandomThread();
  }

  void SuspendThisThread(size_t start_fault_count) {
    if (!this_thread_.IsParkEnabled()) {
      return;
    }

    ThreadWaitQueue::WaitNode this_thread_node;

    {
      std::lock_guard guard(mutex_);

      if (suspended_thread_count_ >= suspended_thread_limit_) {
        // Too many suspended threads
        return;
      }

      if (suspended_thread_count_ + 1 >= thread_count_) {
        // At least one thread should run
        return;
      }

      suspended_threads_.Add(&this_thread_node);
      ++suspended_thread_count_;
    }

    this_thread_node.Await();

    --suspended_thread_count_;

    size_t suspend_time = fault_count_ - start_fault_count;
    WHEELS_UNUSED(suspend_time);
  }

 private:
  std::atomic<size_t> fault_count_{0};

  std::atomic<size_t> thread_count_{0};

  size_t suspended_thread_limit_;

  RandomEvent suspend_;
  RandomEvent resume_;

  ::std::mutex mutex_;
  ThreadWaitQueue suspended_threads_;
  ::std::atomic<size_t> suspended_thread_count_{0};

  Yielder yielder_;
};

/////////////////////////////////////////////////////////////////////

IAdversaryPtr CreateLockFreeAdversary() {
  return std::make_shared<LockFreeAdversary>(10, 2, 5, 10);
}

}  // namespace fault
}  // namespace twist::rt::thr
