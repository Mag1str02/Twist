#pragma once

#include <wheels/test/test.hpp>

#include <memory>

namespace wheels::test {

////////////////////////////////////////////////////////////////////////////////

class ITestFailHandler {
 public:
  virtual ~ITestFailHandler() = default;

  virtual void Fail(const ITest& test, const std::string& error) = 0;
};

}  // namespace wheels::test
