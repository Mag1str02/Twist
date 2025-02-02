#include "pretty_printer.hpp"

#include <iostream>

#include <twist/wheels/fmt/source_location.hpp>
#include <twist/wheels/fmt/buffer.hpp>

#include <twist/trace/fmt.hpp>

#include <backward.hpp>

#include <fmt/format.h>

#include <iostream>

namespace twist::sim {

namespace log {

static void PrintSeparatorLine(std::ostream& out) {
  out << "------------------------------" << std::endl;
}

static void PrintSourceSnippet(std::ostream& out, wheels::SourceLocation source_loc, std::string_view line_comment = {}) {
  static backward::SnippetFactory snippets;

  auto lines = snippets.get_snippet(source_loc.File(), source_loc.Line(), 5);
  for (auto& line : lines) {
    bool current_line = line.first == (uint32_t)source_loc.Line();

    std::string prefix = "  ";
    if (current_line) {
      prefix = "> ";
    }
    out << prefix << line.first << ": " << line.second;
    if (current_line && !line_comment.empty()) {
      out << "  // " << line_comment;
    }
    out << std::endl;
  }

  out << std::endl;
}

static bool IsRuntime(trace::AttrBase* attrs) {
  static const std::string_view kRuntimeFlag = "twist.rt";

  auto* attr = attrs;
  while (attr != nullptr) {
    if (attr->GetName() == kRuntimeFlag) {
      return true;
    }
    attr = attr->next;
  }

  return false;
}

bool IsRuntime(const trace::Scope* scope) {
  if (IsRuntime(scope->GetAttrs())) {
    return true;
  }
  if (auto* dom = scope->GetDomain(); dom != nullptr) {
    return IsRuntime(dom->GetAttrs());
  }
  return false;
}

void PrettyPrinter::Add(Event* event) {
  PrintSeparatorLine(out_);

  {
    fmt::AutoFmtBuffer buffer;

    if (event->sched) {
      buffer.Append("[Thread #{}]", event->sched->thread_id);
      if (event->sched->fiber_id) {
        buffer.Append("[Fiber #{}]", *(event->sched->fiber_id));
      }
      buffer.Append("[Iter {}]", event->sched->iter);
      buffer.Append(" {}", event->descr);
    } else {
      buffer.Append("[Scheduler] {}", event->descr);
    }

    out_ << buffer.StringView() << std::endl;
  }

  if (event->source) {
    out_ << ::fmt::format("> {}:\n", event->source->loc) << std::endl;

    PrintSourceSnippet(out_, event->source->loc, event->source->comment);

    auto* scope = event->scope;

    while (scope != nullptr) {
      if (!IsRuntime(scope)) {
        out_ << ::fmt::format("Scope: {} ({})\n\n", *scope, scope->GetLoc());
        PrintSourceSnippet(out_, scope->GetLoc());
      }
      scope = scope->GetParent();
    }
  }

  /*
  const trace::Scope* scope = event->scope;

  if (scope != nullptr) {
    fmt::AutoFmtBuffer buffer;

    buffer.Append("Scope: ");
    size_t level = 0;
    while (scope != nullptr) {
      if (IsRuntime(scope)) {
        scope = scope->GetParent();
        continue;
      }

      if (level > 0) {
        buffer.Append(" <- ");
      }

      buffer.Append("{} ({})", *scope, scope->GetLoc());

      ++level;
      scope = scope->GetParent();
    }
    buffer.Append("\n");

    out_ << buffer.StringView() << std::endl;
  }
  */
}

PrettyPrinter::PrettyPrinter(std::ostream& out)
    : out_(out) {
}

PrettyPrinter::PrettyPrinter()
    : PrettyPrinter(std::cout) {
}

}  // namespace log

}  // namespace twist::sim
