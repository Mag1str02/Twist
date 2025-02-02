#include <wheels/test/runtime.hpp>

#include <wheels/test/console_reporter.hpp>

#include <wheels/test/here.hpp>
#include <wheels/test/fork.hpp>

#include <wheels/test/support/sanitizers.hpp>

#include <wheels/core/assert.hpp>
#include <wheels/core/exception.hpp>
#include <wheels/core/stop_watch.hpp>

#include <wheels/logging/logging.hpp>

#include <cassert>
#include <iostream>
#include <mutex>

#ifndef __has_feature
#define __has_feature(x) 0
#endif

namespace wheels::test {

//////////////////////////////////////////////////////////////////////

Runtime& Runtime::Access() {
  static Runtime rt;
  return rt;
}

////////////////////////////////////////////////////////////////////////////////

class AbortOnFailHandler : public ITestFailHandler {
 public:
  void Fail(const ITest& test, const std::string& error) override {
    wheels::FlushPendingLogMessages();
    Runtime::Access().Reporter().TestFailed(test, error);
    std::abort();
  }
};

ITestFailHandler& AbortFailHandler() {
  static AbortOnFailHandler instance;
  return instance;
}

//////////////////////////////////////////////////////////////////////

// Setup

Runtime::Runtime()
    : fail_handler_(&AbortFailHandler()),
      reporter_(&ConsoleReporter()) {
  Initialize();
}

void Runtime::DisableStdoutBuffering() {
  std::cout.setf(std::ios::unitbuf);
}

void Runtime::Initialize() {
  DisableStdoutBuffering();
}

//////////////////////////////////////////////////////////////////////

// Headers

void Runtime::PrintCompilerVersion() {
  std::cout << "Compiler: " __VERSION__ << std::endl;
}

void Runtime::PrintSanitizerInfo() {
#if __has_feature(address_sanitizer)
  std::cout << "Sanitizer: Address "
               "(https://clang.llvm.org/docs/AddressSanitizer.html)"
            << std::endl;
#elif __has_feature(thread_sanitizer)
  std::cout
      << "Sanitizer: Thread (https://clang.llvm.org/docs/ThreadSanitizer.html)"
      << std::endl;
#else
  // Do not care
#endif

  int sanitizer_slowdown = GetSanitizerSlowdown();
  if (sanitizer_slowdown != 1) {
    std::cout << "Expected slowdown introduced by sanitizer: x"
              << sanitizer_slowdown << std::endl;
  }
}

void Runtime::PrintTestFrameworkOptions(const GlobalOptions& options) {
  if (options.forks) {
    std::cout << "Run tests in subprocesses (set --disable_forks flag to "
                 "disable forks)"
              << std::endl;
  } else {
    std::cout << "Forks disabled" << std::endl;
  }

  if (options.disable_time_limits) {
    std::cout << "Test time limits disabled" << std::endl;
  }
}

//////////////////////////////////////////////////////////////////////

TestList Runtime::FilterTests(ITestFilterPtr filter) {
  TestList selected;
  for (const auto& test : tests_) {
    if (filter->Accept(test)) {
      selected.push_back(test);
    }
  }
  return selected;
}

TestList Runtime::FilterSuites(std::vector<std::string> suites) {
  TestList selected;

  for (const auto& suite : suites) {
    for (const auto& test : tests_) {
      if (test->Suite() == suite) {
        selected.push_back(test);
      }
    }
  }
  return selected;
}

//////////////////////////////////////////////////////////////////////

static std::chrono::milliseconds ToMillis(std::chrono::nanoseconds ns) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(ns);
}

//////////////////////////////////////////////////////////////////////

const ITest& Runtime::CurrentTest() {
  WHEELS_VERIFY(current_test_, "Not in test context");
  return *current_test_;
}

void Runtime::FailCurrentTest(const std::string& reason) {
  static std::mutex mutex;

  {
    std::lock_guard locker(mutex);
    FailHandler().Fail(CurrentTest(), reason);
  }
}

void Runtime::RunTestImpl(ITestPtr test) {
  if (options_.forks || test->Options().force_fork_) {
    RunTestWithFork(std::move(test), options_);
  } else {
    RunTestHere(std::move(test), options_);
  }
}

void Runtime::RunTest(ITestPtr test) {
  reporter_->TestStarted(*test);

  wheels::StopWatch stop_watch;

  try {
    current_test_ = test;
    RunTestImpl(test);
    current_test_ = nullptr;
  } catch (...) {
    WHEELS_PANIC(
        "Test framework internal error: " << CurrentExceptionMessage());
  }

  reporter_->TestPassed(*test, ToMillis(stop_watch.Elapsed()));
}

void Runtime::RunTests(TestList tests, GlobalOptions options) {
  DisableStdoutBuffering();
  PrintCompilerVersion();
  PrintSanitizerInfo();
  PrintTestFrameworkOptions(options);

  wheels::StopWatch stop_watch;

  options_ = options;

  for (auto&& test : tests) {
    RunTest(test);
  }

  reporter_->AllTestsPassed(tests.size(), ToMillis(stop_watch.Elapsed()));
}

void Runtime::RunTests(ITestFilterPtr filter, GlobalOptions options) {
  RunTests(FilterTests(filter), options);
}

void Runtime::RunSuites(std::vector<std::string> suites, GlobalOptions options) {
  RunTests(FilterSuites(suites), options);
}

}  // namespace wheels::test
