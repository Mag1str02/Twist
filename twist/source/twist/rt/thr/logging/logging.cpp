#include "logging.hpp"

#include <wheels/support/leaks.hpp>

#include <twist/wheels/fmt/source_location.hpp>

#include <twist/trace/fmt.hpp>

#include <fmt/format.h>
#include <fmt/std.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

#include <pthread.h>

namespace twist::rt::thr::log {

//////////////////////////////////////////////////////////////////////

void RestartLoggerInChildAfterFork();

//////////////////////////////////////////////////////////////////////

struct LogEvent {
  std::thread::id thread_id;
  wheels::SourceLocation where;
  std::string scope;
  std::string message;

  std::atomic<LogEvent*> next_;
};

//////////////////////////////////////////////////////////////////////

class LogWriter {
 public:
  void Write(const LogEvent& e) {
    std::cout << Format(e) << std::endl;
  }

 private:
  std::string Format(const LogEvent& e) {
    return fmt::format("T{} -- {} -- {}", e.thread_id, e.scope, e.message);
  }
};

//////////////////////////////////////////////////////////////////////

// Multi-producer/single consumer queue based on lock-free stack

class EventQueue {
 public:
  void Enqueue(LogEvent* event) {
    while (true) {
      LogEvent* top = top_.load();
      event->next_ = top;
      if (top_.compare_exchange_weak(top, event)) {
        return;
      } else {
        std::this_thread::yield();
      }
    }
  }

  LogEvent* DequeueAll() {
    auto* stack = top_.exchange(nullptr);
    return RotateList(stack);
  }

  bool IsEmpty() const {
    return top_ == nullptr;
  }

 private:
  static LogEvent* RotateList(LogEvent* head) {
    if (head == nullptr) {
      return head;
    }

    LogEvent* prev = head;
    LogEvent* current = prev->next_;

    while (current != nullptr) {
      LogEvent* next = current->next_;
      current->next_ = prev;
      prev = current;
      current = next;
    }

    head->next_ = nullptr;

    return prev;
  }

 private:
  std::atomic<LogEvent*> top_{nullptr};
};

//////////////////////////////////////////////////////////////////////

// Lock-free LogEvent pool

class EventPool {
 public:
  LogEvent* TryAcquire() {
    while (true) {
      LogEvent* pool_head = pool_head_.load();

      if (pool_head == nullptr) {
        return nullptr;
      }

      if (pool_head_.compare_exchange_weak(pool_head, pool_head->next_)) {
        return pool_head;
      } else {
        std::this_thread::yield();
      };
    }
  }

  void Release(LogEvent* events) {
    LogEvent* head = events;
    LogEvent* tail = FindTail(events);

    while (true) {
      LogEvent* pool_head = pool_head_.load();
      tail->next_ = pool_head;
      if (pool_head_.compare_exchange_weak(pool_head, head)) {
        return;
      } else {
        std::this_thread::yield();
      }
    }
  }

 private:
  LogEvent* FindTail(LogEvent* events) {
    LogEvent* tail = events;
    while (tail->next_ != nullptr) {
      tail = tail->next_;
    }
    return tail;
  }

 private:
  std::atomic<LogEvent*> pool_head_{nullptr};
};

//////////////////////////////////////////////////////////////////////

class Logger {
 public:
  Logger() {
    Start();
    ScheduleRestartAfterFork();
  }

  void Start() {
    writer_thread_ = std::thread([this]() { WriteLoop(); });
  }

  void Append(trace::Scope* scope, wheels::SourceLocation where, std::string_view message) {
    auto* event = AllocateEvent();

    // fill log event
    event->where = where;
    // TODO: optimize allocations
    event->scope = ::fmt::format("{}", *scope);
    event->message = std::string{message};
    event->thread_id = std::this_thread::get_id();

    pending_events_.Enqueue(event);
  }

  void SynchronousFlush() {
    AwaitFlush();
  }

 private:
  LogEvent* AllocateEvent() {
    auto* event = event_pool_.TryAcquire();
    if (event) {
      return event;
    } else {
      return new LogEvent{};
    }
  }

  void Release(LogEvent* events) {
    event_pool_.Release(events);
  }

  void ScheduleRestartAfterFork() {
    pthread_atfork(nullptr, nullptr, RestartLoggerInChildAfterFork);
  }

  void AwaitFlush() {
    size_t write_count = write_count_.load();

    while (write_count_.load() < write_count + 2) {
      std::this_thread::yield();
    }
  }

  void WriteLoop() {
    static const auto kPollPeriod = std::chrono::microseconds(100);

    while (true) {
      LogEvent* events = pending_events_.DequeueAll();
      if (events != nullptr) {
        Write(events);
        ++write_count_;
      } else {
        ++write_count_;
        std::this_thread::sleep_for(kPollPeriod);
      }
    }
  }

  void Write(LogEvent* events) {
    DoWrite(events);
    Release(events);
  }

  void DoWrite(const LogEvent* events) {
    for (const LogEvent* event = events; event; event = event->next_) {
      if (!event->message.empty()) {
        writer_.Write(*event);
      }
    }
  }

 private:
  std::thread writer_thread_;
  EventQueue pending_events_;
  std::atomic<size_t> write_count_{0};
  EventPool event_pool_;
  LogWriter writer_;
};

//////////////////////////////////////////////////////////////////////

Logger* GetLogger() {
  static Logger* logger;
  static std::once_flag created;

  std::call_once(created, [&]() {
    wheels::LeaksScope allow_leaks;
    logger = new Logger();
  });

  return logger;
}

//////////////////////////////////////////////////////////////////////

void RestartLoggerInChildAfterFork() {
  GetLogger()->Start();
}

//////////////////////////////////////////////////////////////////////

// API

void LogMessage(trace::Scope* scope, wheels::SourceLocation where, std::string_view message) {
  GetLogger()->Append(scope, where, message);
}

void FlushPendingLogMessages() {
  GetLogger()->SynchronousFlush();
}

}  // namespace twist::rt::thr::log
