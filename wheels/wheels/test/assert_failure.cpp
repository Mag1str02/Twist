#include <wheels/test/assert_failure.hpp>

namespace wheels::test {

std::ostream& operator<<(std::ostream& out, const AssertionFailure& failure) {
  out << "Assertion '" << failure.condition_ << "' failed at "
      << failure.where_;
  auto description = failure.GetDescription();
  if (!description.empty()) {
    out << ": " << description;
  }
  return out;
}

}  // namespace wheels::test
