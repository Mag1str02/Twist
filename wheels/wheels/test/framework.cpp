#include <wheels/test/framework.hpp>

#include <wheels/core/exception.hpp>
#include <wheels/core/string_builder.hpp>

#include <wheels/test/runtime.hpp>

#include <cstdlib>
#include <string>
#include <thread>

namespace wheels::test {

////////////////////////////////////////////////////////////////////////////////

void RegisterTest(ITestPtr test) {
  Runtime::Access().Register(std::move(test));
}

////////////////////////////////////////////////////////////////////////////////

const ITest& CurrentTest() {
  return Runtime::Access().CurrentTest();
}

////////////////////////////////////////////////////////////////////////////////

void FailTest(const std::string& reason) {
  Runtime::Access().FailCurrentTest(reason);
}

void FailTestByAssert(const AssertionFailure& failure) {
  FailTest(failure.ToString());
}

static std::string FormatCurrentExceptionMessage() {
  return StringBuilder() << "Uncaught exception in thread "
                         << std::this_thread::get_id() << ": "
                         << CurrentExceptionMessage();
}

void FailTestByException() {
  FailTest(FormatCurrentExceptionMessage());
}

}  // namespace wheels::test
