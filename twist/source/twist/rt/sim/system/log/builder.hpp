#pragma once

#include "../thread/struct.hpp"

#include "buffer.hpp"
#include "fmt_writer.hpp"
#include "event.hpp"

#include <fmt/core.h>
#include <fmt/format.h>

#include <optional>

namespace twist::rt::sim {

namespace system {

namespace log {

class EventBuilder {
 public:
  EventBuilder()
      : descr_{Buffer()},
        comment_{CommentBuffer()} {
  }

  FmtWriter DescrWriter() {
    return {&descr_};
  }

  FmtWriter LineCommentWriter() {
    return {&comment_};
  }

  template <typename... Args>
  EventBuilder& Descr(::fmt::format_string<Args...> format_str, Args&&... args) {
    descr_.Reset();
    descr_.template Append(format_str, std::forward<Args>(args)...);
    return *this;
  }

  template <typename... Args>
  EventBuilder& SetComment(::fmt::format_string<Args...> format_str, Args&&... args) {
    comment_.Reset();
    comment_.template Append(format_str, std::forward<Args>(args)...);
    return *this;
  }

  EventBuilder& SetSourceLoc(wheels::SourceLocation source_loc) {
    source_loc_.emplace(source_loc);
    return *this;
  }

  EventBuilder& SetScope(Thread* thread) {
    scope_ = thread->fiber->scope;
    return *this;
  }

  void Done() {
    event_.descr = DescrView();

    event_.scope = scope_;

    event_.user = false;

    if (source_loc_) {
      event_.source.emplace();
      event_.source->loc = *source_loc_;
      event_.source->comment = CommentView();
    }
  }

  Event* EventView() {
    return &event_;
  }

 private:
  std::string_view DescrView() const {
    return descr_.StringView();
  }

  std::string_view CommentView() const {
    return comment_.StringView();
  }

 private:
  FmtBuffer descr_;
  FmtBuffer comment_;
  std::optional<wheels::SourceLocation> source_loc_;
  trace::Scope* scope_ = nullptr;
  Event event_;
};

}  // namespace log

}  // namespace system

}  // namespace twist::rt::sim
