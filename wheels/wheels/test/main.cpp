#include <wheels/test/main.hpp>

#include <wheels/test/runtime.hpp>

#include <gflags/gflags.h>

DEFINE_string(suite, ".*", "Regexp for test suite name");
DEFINE_string(test, ".*", "Regexp for test name");
DEFINE_bool(disable_forks, false, "Do not fork tests");
DEFINE_bool(disable_time_limits, false, "Disable time limits (for debugging)");

namespace wheels::test {

static GlobalOptions MakeOptions(bool forks, bool no_time_limits) {
  GlobalOptions options;
  options.forks = forks;
  options.disable_time_limits = no_time_limits;
  return options;
}

void RunTestsMain(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  auto filter = CreateTestFilter(FLAGS_suite, FLAGS_test);
  auto options = MakeOptions(!FLAGS_disable_forks, FLAGS_disable_time_limits);

  Runtime::Access().RunTests(filter, options);
}

}  // namespace wheels::test
