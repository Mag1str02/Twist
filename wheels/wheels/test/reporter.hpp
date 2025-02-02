#pragma once

#include <wheels/test/test.hpp>

#include <chrono>

namespace wheels::test {

////////////////////////////////////////////////////////////////////////////////

class ITestReporter {
 public:
  virtual ~ITestReporter() = default;

  virtual void TestStarted(const ITest& test) = 0;
  virtual void TestFailed(const ITest& test, const std::string& error) = 0;
  virtual void TestPassed(const ITest& test, std::chrono::milliseconds elapsed) = 0;
  virtual void AllTestsPassed(size_t test_count, std::chrono::milliseconds elapsed) = 0;
};

using ITestReporterPtr = std::shared_ptr<ITestReporter>;

}  // namespace wheels::test
