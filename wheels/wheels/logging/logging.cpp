#include <wheels/logging/logging.hpp>

#include <wheels/support/leaks.hpp>

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>

#include <pthread.h>

namespace wheels {

//////////////////////////////////////////////////////////////////////

void RestartLoggerInChildAfterFork();

//////////////////////////////////////////////////////////////////////

struct LogEvent {
  std::thread::id thread_id_;
  SourceLocation where_;
  std::string message_;

  std::atomic<LogEvent*> next_;
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

static int GetMinimalLogLevelFromEnv(LogLevel default_level) {
  const char* env_level = getenv("WHEELS_LOG_LEVEL");
  if (env_level) {
    try {
      return std::atoi(env_level);
    } catch (...) {
      // write to stderr?
    }
  }
  return (int)default_level;
}

//////////////////////////////////////////////////////////////////////

class Logger {
 public:
  Logger() {
    InitMinLevel();
    Start();
    ScheduleRestartAfterFork();
  }

  void Start() {
    writer_thread_ = std::thread([this]() { WriteLoop(); });
  }

  void Append(SourceLocation&& where, std::string&& message) {
    auto* event = AllocateEvent();

    // fill log event
    event->where_ = where;
    event->message_ = std::move(message);
    event->thread_id_ = std::this_thread::get_id();

    pending_events_.Enqueue(event);
  }

  void SynchronousFlush() {
    AwaitFlush();
  }

  bool Accepted(LogLevel level) const {
    return (int)level >= min_level_;
  }

 private:
  void InitMinLevel() {
    min_level_ = GetMinimalLogLevelFromEnv(LogLevel::Trace);
  }

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
    std::cout << Format(events) << std::flush;
    Release(events);
  }

  std::string Format(const LogEvent* events) {
    std::stringstream out;

    for (const LogEvent* event = events; event; event = event->next_) {
      if (!event->message_.empty()) {
        Format(*event, out);
      }
    }

    return out.str();
  }

  static void Format(const LogEvent& event, std::stringstream& out) {
    out << "[T " << event.thread_id_ << "]\t" << event.message_ << std::endl;
  }

 private:
  int min_level_;
  std::thread writer_thread_;
  EventQueue pending_events_;
  std::atomic<size_t> write_count_{0};
  EventPool event_pool_;
};

//////////////////////////////////////////////////////////////////////

Logger* GetLogger() {
  static Logger* logger;
  static std::once_flag created;

  std::call_once(created, [&]() {
    LeaksScope allow_leaks;
    logger = new Logger();
  });

  return logger;
}

bool LevelAccepted(LogLevel level) {
  return GetLogger()->Accepted(level);
}

void LogMessage(SourceLocation where, std::string message) {
  GetLogger()->Append(std::move(where), std::move(message));
}

void FlushPendingLogMessages() {
  GetLogger()->SynchronousFlush();
}

//////////////////////////////////////////////////////////////////////

void RestartLoggerInChildAfterFork() {
  GetLogger()->Start();
}

}  // namespace wheels
