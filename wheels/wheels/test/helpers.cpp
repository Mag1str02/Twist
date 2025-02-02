#include <wheels/test/helpers.hpp>

#include <wheels/core/string_builder.hpp>

#include <algorithm>
#include <iomanip>

namespace wheels::test {

static bool IsMultiLine(const std::string& error) {
  return std::count(error.begin(), error.end(), '\n') > 1;
}

std::string FormatStderrForErrorMessage(const std::string& err) {
  if (err.empty()) {
    return "<empty>";
  }
  if (!IsMultiLine(err)) {
    return err;
  }
  // Multi-line err
  // TODO: stringstream
  return std::string("\n") + err + "\n";
}

double ToSeconds(std::chrono::milliseconds d) {
  return std::chrono::duration<double>(d).count();
}

std::string FormatSeconds(std::chrono::milliseconds d, size_t digits) {
  return StringBuilder() << std::fixed << std::setprecision(digits)
                         << ToSeconds(d);
}

}  // namespace wheels::test
