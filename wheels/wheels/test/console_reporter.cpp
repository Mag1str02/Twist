#include <wheels/test/console_reporter.hpp>

#include <wheels/test/emoji.hpp>
#include <wheels/test/helpers.hpp>

#include <wheels/test/support/terminal.hpp>
#include <wheels/core/string_utils.hpp>

#include <iostream>

using namespace std::chrono_literals;

namespace wheels::test {

class ConsoleTestReporter : public ITestReporter {
 public:
  void TestStarted(const ITest& test) override {
    PrintSeparatorLine();
    std::cout << "Test " << MAGENTA(Quoted(test.Name())) << " from suite "
              << CYAN(Quoted(test.Suite())) << " is running..." << std::endl;
  }

  void TestFailed(const ITest& test, const std::string& error) override {
    std::cout << "Test " << MAGENTA(Quoted(test.Name())) << RED(" FAILED ")
              << GetFailEmoji() << ": " << error << std::endl
              << std::flush;
  }

  void TestPassed(const ITest& /*test*/, std::chrono::milliseconds elapsed) override {
    static const auto kSignificantTestTime = 10ms;

    std::cout << GREEN("PASSED");
    if (elapsed > kSignificantTestTime) {
      std::cout << " (" << FormatSeconds(elapsed, 2) << " seconds)";
    }
    std::cout << std::endl;
  }

  void AllTestsPassed(size_t test_count, std::chrono::milliseconds elapsed) override {
    if (test_count == 0) {
      std::cout << "NO TESTS TO RUN..." << std::endl;
      return;
    }

    PrintSeparatorLine();

    if (test_count > 1) {
      std::cout << GREEN("ALL " << test_count << " TESTS PASSED!");
    } else {
      std::cout << GREEN("1 TEST PASSED!");
    }
    std::cout << " (Total time: " << FormatSeconds(elapsed, 2) << " seconds)"
              << std::endl
              << GetSuccessEmoji() << std::endl
              << std::flush;
  }

 private:
  static void PrintSeparatorLine() {
    static const std::string kSeparatorLine(80, '-');
    std::cout << kSeparatorLine << std::endl;
  }
};

////////////////////////////////////////////////////////////////////////////////

ITestReporter& ConsoleReporter() {
  static ConsoleTestReporter instance;
  return instance;
}

}  // namespace wheels::test
