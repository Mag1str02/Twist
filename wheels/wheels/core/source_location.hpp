#pragma once

#include <ostream>

namespace wheels {

// Represents location in source code: file name, function name, line number

class SourceLocation {
 public:
  SourceLocation() = default;

  SourceLocation(const char* file, const char* function, int line)
      : file_(file), function_(function), line_(line) {
  }

  static SourceLocation Current(const char* file = __builtin_FILE(),
                                const char* function = __builtin_FUNCTION(),
                                const int line = __builtin_LINE()) {
    return {file, function, line};
  }

  const char* File() const {
    return file_;
  }

  const char* Function() const {
    return function_;
  }

  int Line() const {
    return line_;
  }

 private:
  const char* file_;
  const char* function_;
  int line_;
};

std::ostream& operator<<(std::ostream& out, const SourceLocation& where);

inline SourceLocation Here(SourceLocation loc = SourceLocation::Current()) {
  return loc;
}

}  // namespace wheels

// Use WHEELS_HERE in macros to capture current location

#define WHEELS_HERE \
  ::wheels::SourceLocation(__FILE__, __PRETTY_FUNCTION__, __LINE__)
