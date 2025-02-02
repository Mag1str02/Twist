#pragma once

#include <wheels/test/test.hpp>
#include <wheels/test/filter.hpp>
#include <wheels/test/fail_handler.hpp>
#include <wheels/test/reporter.hpp>
#include <wheels/test/test_options.hpp>

namespace wheels::test {

class Runtime {
 public:
  Runtime();

  static Runtime& Access();

  void Register(ITestPtr test) {
    tests_.push_back(std::move(test));
  }

  TestList AllTests() const {
    return tests_;
  }

  void InstallFailHandler(ITestFailHandler& handler) {
    fail_handler_ = &handler;
  }

  ITestFailHandler& FailHandler() {
    return *fail_handler_;
  }

  ITestReporter& Reporter() {
    return *reporter_;
  }

  const ITest& CurrentTest();

  void FailCurrentTest(const std::string& reason);

  void RunTests(ITestFilterPtr filter, GlobalOptions options);
  void RunSuites(std::vector<std::string> suites, GlobalOptions options);

 private:
  // Setup
  static void DisableStdoutBuffering();
  void Initialize();

  // Header
  static void PrintCompilerVersion();
  static void PrintSanitizerInfo();
  static void PrintTestFrameworkOptions(const GlobalOptions& options);

  TestList FilterTests(ITestFilterPtr filter);
  TestList FilterSuites(std::vector<std::string> names);

  void RunTestImpl(ITestPtr test);
  void RunTest(ITestPtr test);

  void RunTests(TestList tests, GlobalOptions options);

 private:
  // Repository
  std::vector<ITestPtr> tests_;

  GlobalOptions options_;

  ITestFailHandler* fail_handler_;
  ITestReporter* reporter_;

  ITestPtr current_test_;
};

}  // namespace wheels::test
