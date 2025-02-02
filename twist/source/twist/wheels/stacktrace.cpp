#include "stacktrace.hpp"

#include <wheels/core/compiler.hpp>

#if defined(__TWIST_SIM_DEBUG__)
#define BACKWARD_HAS_BFD 1
#endif

#include <backward.hpp>

using namespace backward;

namespace twist::rt {

#if !defined(__TWIST_SIM_DEBUG__)

void PrintStackTrace(std::ostream& out) {
  static const size_t kDepth = 64;

  StackTrace st;
  st.load_here(kDepth);

  SnippetFactory snippets;

  TraceResolver tr;
  tr.load_stacktrace(st);
  for (size_t i = 0; i < st.size(); ++i) {
    ResolvedTrace trace = tr.resolve(st[i]);
    out << "#"
        << i
        //<< " " << trace.object_filename
        << " " << trace.object_function << " [" << trace.addr << "]"
        << std::endl;
  }
}

#elif 1

static const std::vector<std::string> kFilenamePatternStopList = {
    "include/c++",
    "twist/ed/",
    "twist/rt/",
    "twist/test/",
    "twist/wheels/",
    "_deps/"
};

static bool IsProbablyUserFrame(const ResolvedTrace& trace) {
  for (const auto& pattern : kFilenamePatternStopList) {
    if (trace.source.filename.find(pattern) != std::string::npos) {
      return false;
    }
  }
  return true;
}

void PrintStackTrace(std::ostream& out) {
  static const size_t kDepth = 64;

  StackTrace st;
  st.load_here(kDepth);

  TraceResolver tr;
  tr.load_stacktrace(st);

  size_t frames_printed = 0;

  SnippetFactory snippets;
  for (size_t i = 0; i < st.size(); ++i) {
    backward::ResolvedTrace trace = tr.resolve(st[i]);

    if (!IsProbablyUserFrame(trace)) {
      continue;
    }

    auto lines =
        snippets.get_snippet(trace.source.filename, trace.source.line, 3);

    if (lines.empty()) {
      continue;
    }

    if (frames_printed > 0) {
      out << std::endl;
    }

    out << "#"
        << frames_printed
        //<< " " << trace.object_filename
        << " " << trace.object_function
        << std::endl;

    for (auto& line : lines) {
      std::string prefix = "  ";
      if (line.first == trace.source.line) {
        prefix = "> ";
        //out << wheels::terminal::Magenta();
      }
      out << prefix << line.first << ": " << line.second << std::endl;
      //out << wheels::terminal::Reset();
    }

    ++frames_printed;
  }
}

#else

void PrintStackTrace(std::ostream& out) {
  WHEELS_UNUSED(out);
  // Nop
}

#endif

}  // namespace twist::rt
