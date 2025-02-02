#pragma once

#include <wheels/core/preprocessor.hpp>
#include <wheels/core/source_location.hpp>

#include <sstream>
#include <string>

namespace wheels::test {

class AssertionFailure {
 public:
  AssertionFailure(const std::string& condition, const SourceLocation& where)
      : condition_(condition), where_(where) {
  }

  template <typename T>
  AssertionFailure& operator<<(const T& next) {
    description_ << next;
    return *this;
  }

  const SourceLocation& GetLocation() const {
    return where_;
  }

  std::string GetDescription() const {
    return description_.str();
  }

  friend std::ostream& operator<<(std::ostream& out,
                                  const AssertionFailure& error);

  std::string ToString() const {
    std::ostringstream out;
    out << *this;
    return out.str();
  }

 private:
  std::string condition_;
  SourceLocation where_;
  std::ostringstream description_;
};

std::ostream& operator<<(std::ostream& out, const AssertionFailure& failure);

}  // namespace wheels::test

// Used by test macros (ASSERT_TRUE etc)
#define ASSERTION_FAILURE(cond) \
  ::wheels::test::AssertionFailure(WHEELS_TO_STRING(cond), WHEELS_HERE)
