#include <twist/cross.hpp>

#include <twist/trace/domain.hpp>
#include <twist/trace/scope.hpp>
#include <twist/trace/attr/uint.hpp>

#include <functional>
#include <optional>
#include <queue>

//////////////////////////////////////////////////////////////////////

using Task = std::function<void()>;

class TaskQueue {
 public:
  TaskQueue() {
    queue_.SetVisibility(-1);
  }

  void Push(Task task) {
    twist::trace::Scope push{queue_};

    tasks_.push(task);
  }

  std::optional<Task> TryPop() {
    twist::trace::Scope try_pop{queue_};

    if (tasks_.empty()) {
      try_pop.Here();
      return std::nullopt;
    } else {
      try_pop.Here();
      Task front = tasks_.front();
      tasks_.pop();
      return front;
    }
  }

 private:
  std::queue<Task> tasks_;
  twist::trace::Domain queue_{"TaskQueue"};
};

class ThreadPool {
 public:
  ThreadPool() {
    pool_.SetVisibility(+1);
  }

  void Submit(Task task) {
    twist::trace::Scope submit{pool_};
    submit.Here();
    tasks_.Push(task);
  }

  void Work() {
    twist::trace::Scope work{pool_};

    size_t task_count = 0;

    while (auto task = tasks_.TryPop()) {
      // Inherits domain from the parent scope
      twist::trace::Scope iter{"WorkLoopIter"};
      twist::trace::attr::Uint index_attr{"iter", task_count};
      iter.LinkAttr(index_attr);

      iter.Note("Do task");
      (*task)();

      ++task_count;
    }
  }

 private:
  TaskQueue tasks_;
  twist::trace::Domain pool_{"ThreadPool"};
};

//////////////////////////////////////////////////////////////////////

int main() {
  twist::cross::Run([] {
    twist::trace::Domain example{"Example"};

    ThreadPool pool;
    pool.Submit([&] {
      // Explicit scope name
      twist::trace::Scope task{example, "Task"};
      task.SetVisibility(2);

      task.Here();

      pool.Submit([] {
      });
    });
    pool.Work();
  });

  return 0;
}
