#pragma once

#include <twist/trace/scope.hpp>

#include <wheels/core/source_location.hpp>

#include <optional>
#include <string_view>

namespace twist::rt::sim {

namespace system {

namespace log {

struct SourceContext {
  wheels::SourceLocation loc;
  std::string_view comment;
};

struct SchedContext {
  uint64_t thread_id;
  std::optional<uint64_t> fiber_id;
  uint64_t iter;
};

struct Event {
  std::string_view descr;

  bool user = false;
  trace::Scope* scope = nullptr;

  // User context
  std::optional<SchedContext> sched;
  std::optional<SourceContext> source;
};

}  // namespace log

}  // namespace system

}  // namespace twist::rt::sim
